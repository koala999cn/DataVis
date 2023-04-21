#include "KcImOglPaint.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "glad.h"
#include "KcVertexDeclaration.h"
#include "KtLineS2d.h"
#include "KuMesh.h"
#include "KuMath.h"
#include "opengl/KcGlslProgram.h"
#include "opengl/KcGlslShader.h"
#include "opengl/KcGpuBuffer.h"
#include "opengl/KcPointObject.h"
#include "opengl/KcLineObject.h"
#include "opengl/KcEdgedObject.h"
#include "opengl/KcMarkerObject.h"
#include "opengl/KcTextObject.h"
#include "opengl/KsShaderManager.h"
#include "opengl/KuOglUtil.h"
#include "plot/KpContext.h"
#include "KuPrimitiveFactory.h"
#include "KtGeometryImpl.h"
#include "layout/KuLayoutUtil.h"


namespace kPrivate
{
	static void oglDrawRenderList(const ImDrawList*, const ImDrawCmd* cmd)
	{
		auto paint = (KcImOglPaint*)cmd->UserCallbackData;
		paint->drawRenderList_();
	}
}


KcImOglPaint::KcImOglPaint(camera_type& cam)
	: super_(cam)
{

}


void KcImOglPaint::saveObjList_()
{
	savedObjList_.clear();

	for (auto& r : renderList_) 
		for (auto& o : r.second.objs)
			savedObjList_[o.get()] = o;
}


void KcImOglPaint::beginPaint()
{
	saveObjList_();

	renderList_.clear();

	viewportHistList_.clear();
	clipRectHistList_.clear();
	
	viewportHistList_.push_back(viewport());
	curViewport_ = 0;

	auto crmin = ImGui::GetWindowDrawList()->GetClipRectMin();
	auto crmax = ImGui::GetWindowDrawList()->GetClipRectMax();
	clipRectHistList_.emplace_back(point2(crmin.x, crmin.y), point2(crmax.x, crmax.y));
	clipRectStack_.assign(1, 0);
	
	polygonOffset_ = false;

	clipBoxHistList_.clear();
	curClipBox_ = -1;

	// NB: 须在此处清空深度缓存
	// 若放到configOglState_，则会与ImGui的clipRect设置冲突（原因不明），造成除legend之外的其他元素不能显示
	// 若不调用ImGui的PushClipRect则一切正常
	if (depthTest())
	    glClear(GL_DEPTH_BUFFER_BIT);

	super_::beginPaint();
}


namespace kPrivate
{
	void test(const ImDrawList*, const ImDrawCmd* cmd)
	{
		return;
		//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(0);
		auto texId = ImGui::GetIO().Fonts->TexID;
		assert(texId == cmd->GetTexID());
		glEnable(GL_TEXTURE);
		glActiveTexture(GL_TEXTURE0);
		assert(glIsTexture((GLuint)(intptr_t)cmd->GetTexID()));
		GLint params;
		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_RESIDENT, &params);
		assert(params == GL_TRUE);
		glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)cmd->GetTexID());

		glBegin(GL_QUADS);

		glTexCoord2f(0, 0);
		glVertex2f(0, 0);
		glColor3f(1, 0, 1);

		glTexCoord2f(0, 1);
		glVertex2f(0, 1);
		glColor3f(1, 0, 1);

		glTexCoord2f(1, 1);
		glVertex2f(1, 1);
		glColor3f(1, 0, 1);

		glTexCoord2f(1, 0);
		glVertex2f(1, 0);
		glColor3f(1, 0, 1);

		glEnd();
	}
}


void KcImOglPaint::endPaint()
{
	if (!renderList_.empty()) {
		auto dl = ImGui::GetWindowDrawList();
		dl->AddCallback(kPrivate::oglDrawRenderList, this);
		dl->AddCallback(ImDrawCallback_ResetRenderState, nullptr); // 让imgui恢复渲染状态
	}

	super_::endPaint();
}


KcImOglPaint::point3 KcImOglPaint::toNdc_(const point3& pt) const
{
	switch (currentCoord())
	{
	case k_coord_local:
	{
		auto p = camera_.localToNdc(vec4d(pt, 1));
		return p.vec3();
	}

	case k_coord_world:
	{
		auto p = camera_.worldToNdc(vec4d(pt, 1));
		return p.vec3();
	}

	case k_coord_screen:
	{
		auto p = camera_.screenToNdc(vec4d(pt, 1));
		return p.vec3();
	}

	case k_coord_local_screen:
	{
		auto p = camera_.localToWorld(vec4d(pt, 1));
		p = camera_.screenToNdc(p);
		return p.vec3();
	}

	}

	assert(false);
	return point3(0);
}


void KcImOglPaint::drawMarker(const point3& pt)
{
	super_::drawMarker(pt);
}


void* KcImOglPaint::drawPoints_(point_getter fn, unsigned count)
{
	auto obj = new KcPointObject;

	auto decl = std::make_shared<KcVertexDeclaration>();
	decl->pushAttribute(KcVertexAttribute::k_float3, KcVertexAttribute::k_position);

	auto vbo = std::make_shared<KcGpuBuffer>();
	std::vector<point3f> vtx;
	for (unsigned i = 0; i < count; i++) // 装配数据
		vtx.push_back(fn(i));
	vbo->setData(vtx.data(), vtx.size() * sizeof(point3f), KcGpuBuffer::k_stream_draw);

	obj->pushVbo(vbo, decl);
	obj->setColor(clr_);
	obj->setSize(markerSize_);
	pushRenderObject_(obj);
	return obj;
}


void KcImOglPaint::drawCircles_(point_getter fn, unsigned count)
{
	int segments = 10;
	auto geom = std::make_shared<KtGeometryImpl<point3f, unsigned>>(k_triangles);
	geom->reserve(count * (segments + 1)/*加上1个圆点*/, count * segments * 3);

	for (unsigned i = 0; i < count; i++) {
		auto idxBase = geom->vertexCount();
		auto vtxBuf = geom->newVertex(segments + 1);
		auto pt = projectp(fn(i));
		*vtxBuf++ = pt;
		KuPrimitiveFactory::makeCircle10<float>(point3f(pt), markerSize_, vtxBuf);

		auto idxBuf = geom->newIndex(segments * 3);
		for (int i = 1; i < segments; i++) {
			*idxBuf++ = idxBase; // 圆点
			*idxBuf++ = idxBase + i;
			*idxBuf++ = idxBase + i + 1;
		}

		*idxBuf++ = idxBase; // 圆点
		*idxBuf++ = idxBase + segments;
		*idxBuf++ = idxBase + 1;
	}

	auto decl = std::make_shared<KcVertexDeclaration>();
	decl->pushAttribute(KcVertexAttribute::k_float3, KcVertexAttribute::k_position);
	pushCoord(k_coord_screen);
	drawGeom(decl, geom);

	if (edged_) { // 描边
#if 0
		auto obj = lastRenderObject_();
		auto vbo = obj->vbo(); // 共用vbo
		auto edgeObj = new KcLineObject(k_lines);
		edgeObj->setVBO(vbo, decl);
		edgeObj->setWidth(lineWidth_);
		edgeObj->setStyle(lineStyle_);

		// 构建边的index缓存
		auto markerCount = geom->vertexCount() / (segments + 1);
		std::vector<std::uint32_t> edgeIdx(markerCount * segments * 2);
		unsigned idxBase = 0;
		auto edgeIdxBuf = edgeIdx.data();
		for (int i = 0; i < markerCount; i++) {
			// idxBase为圆点，索引从idxBase + 1开始
			for (unsigned j = idxBase + 1; j < idxBase + segments; j++) {
				*edgeIdxBuf++ = j;
				*edgeIdxBuf++ = j + 1;
			}
			*edgeIdxBuf++ = idxBase + segments;
			*edgeIdxBuf++ = idxBase + 1;

			idxBase += segments + 1;
		}

		auto idxBuf = std::make_shared<KcGpuBuffer>(KcGpuBuffer::k_index_buffer);
		idxBuf->setData(edgeIdx.data(), edgeIdx.size() * 4, KcGpuBuffer::k_stream_draw);
		edgeObj->setIBO(idxBuf, edgeIdx.size());

		obj->setColor(secondaryClr_);
		pushRenderObject_(edgeObj);
#endif
		assert(false);
	}

	popCoord();
}


namespace kPrivate
{
	template<int N, bool forceLines = false>
	void drawPolyMarkers_(KvPaint& paint, KvPaint::point_getter fn, unsigned count, 
		const KvPaint::point2 poly[N], float markerSize)
	{
		KePrimitiveType type;
		if constexpr (forceLines)
			type = k_lines;
		else if constexpr (N == 3)
			type = k_triangles;
		else if constexpr (N == 4)
			type = k_quads;
		else
			type = k_lines;

		auto geom = std::make_shared<KtGeometryImpl<point3f, unsigned>>(type);
		geom->reserve(count * N, 0);

		KvPaint::point3 vtx[N];
		for (int i = 0; i < N; i++) {
			auto pt = poly[i] * markerSize;
			vtx[i] = { pt.x(), pt.y(), 0 };
		}

		for (unsigned i = 0; i < count; i++) {
			auto vtxBuf = geom->newVertex(N);
			auto pt = paint.projectp(fn(i));
			for (int i = 0; i < N; i++)
				vtxBuf[i] = pt + vtx[i];
		}

		paint.pushCoord(KvPaint::k_coord_screen);
		paint.drawGeomSolid(geom);
		paint.popCoord();
	}
}


void KcImOglPaint::addMarkers_(point_getter fn, unsigned count, const point2* fillVtx, unsigned numFill,
	const point2* outlineVtx, unsigned numOutline)
{
	assert(numFill > 0);

	auto vbo = newColorVbo_(count * (numFill + numOutline));

	std::vector<point3> vtx;
	vtx.reserve(numFill + numOutline);
	for (unsigned i = 0; i < numFill; i++) {
		auto pt = fillVtx[i] * markerSize_;
		vtx.emplace_back(pt.x(), pt.y(), 0);
	}
	for (unsigned i = 0; i < numOutline; i++) {
		auto pt = outlineVtx[i] * markerSize_;
		vtx.emplace_back(pt.x(), pt.y(), 0);
	}


	for (unsigned i = 0; i < count; i++) {
		auto pt = projectp(fn(i)); // 转换到屏幕坐标

		for (unsigned j = 0; j < numFill; j++) {
			vbo->pos = pt + vtx[j];
			vbo->clr = clr_;
			++vbo;
		}

		for (unsigned j = 0; j < numOutline; j++) {
			vbo->pos = pt + vtx[j + numFill];
			vbo->clr = secondaryClr_;
			++vbo;
		}
	}
}


void KcImOglPaint::addQuadFilled_(const point3& p0, const point3& p1, const point3& p2, const point3& p3, const float4& clr)
{
	auto vbo = newColorVbo_(6);

	vbo[0].pos = p0; vbo[0].clr = clr;
	vbo[1].pos = p1; vbo[1].clr = clr;
	vbo[2].pos = p2; vbo[2].clr = clr;

	vbo[3] = vbo[0];
	vbo[4] = vbo[2];
	vbo[5].pos = p3; vbo[5].clr = clr;
}


void KcImOglPaint::addLine_(const point3& pt0, const point3& pt1, const float4& clr)
{
	auto dp = pt1 - pt0;
	dp.z() = 0;
	dp.normalize();
	dp *= lineWidth_ * 0.5;

	addQuadFilled_(
		{ pt0.x() - dp.y(), pt0.y() + dp.x(), pt0.z() },
		{ pt0.x() + dp.y(), pt0.y() - dp.x(), pt0.z() },
		{ pt1.x() + dp.y(), pt1.y() - dp.x(), pt1.z() },
		{ pt1.x() - dp.y(), pt1.y() + dp.x(), pt1.z() }, clr);
}


void KcImOglPaint::addConvexPolyFilled_(point_getter fn, unsigned count, const float4& clr)
{
	assert(count >= 3);

	auto vbo = newColorVbo_((count - 2) * 3);

	vbo[0].pos = fn(0); vbo[0].clr = clr;
	vbo[1].pos = fn(1); vbo[1].clr = clr;
	vbo[2].pos = fn(2); vbo[2].clr = clr;
	for (unsigned j = 3; j < count; j++) {
		vbo[j] = vbo[0];
		vbo[j + 1] = vbo[j - 1];
		vbo[j + 2].pos = fn(j); vbo[j + 2].clr = clr;
	}
}


void KcImOglPaint::addLineLoop_(point_getter fn, unsigned count, const float4& clr)
{
	for (unsigned j = 1; j < count; j++)
		addLine_(fn(j - 1), fn(j), clr);
	addLine_(fn(count - 1), fn(0), clr);
}


void KcImOglPaint::addMarkers_(point_getter fn, unsigned count, const point2* vtxBuf, unsigned numVtx)
{
	assert(numVtx >= 3);

	std::vector<point3> vtx;
	vtx.reserve(numVtx);
	for (unsigned i = 0; i < numVtx; i++) {
		auto pt = vtxBuf[i] * markerSize_;
		vtx.emplace_back(pt.x(), pt.y(), 0);
	}

	point3 pt;
	auto fns = [&pt, &vtx](unsigned n) {
		return pt + vtx[n];
	};

	for (unsigned i = 0; i < count; i++) {
		pt = projectp(fn(i)); // 转换到屏幕坐标

		addConvexPolyFilled_(fns, numVtx, clr_);

		if (edged_)
			addLineLoop_(fns, numVtx, secondaryClr_);
	}
}


void KcImOglPaint::drawQuadMarkers_(point_getter fn, unsigned count, const point2 quad[4])
{
	kPrivate::drawPolyMarkers_<4>(*this, fn, count, quad, markerSize_);
}


void KcImOglPaint::drawTriMarkers_(point_getter fn, unsigned count, const point2 tri[3])
{
	kPrivate::drawPolyMarkers_<3>(*this, fn, count, tri, markerSize_);
}


KpMarker KcImOglPaint::marker() const
{
	KpMarker m;
	m.type = markerType_;
	m.size = markerSize_;
	m.fill = clr_;
	m.outline = secondaryClr_;
	m.weight = lineWidth_;
	m.showFill = filled_;
	m.showOutline = edged_;
	return m;
}


void* KcImOglPaint::drawMarkers(point_getter fn, unsigned count)
{
	auto obj = new KcMarkerObject;
	auto scale = camera_.screenToNdc({ 1, 1, 1, 0 });
	//assert(KuMath::almostEqual(scale.length(), 1.));
	obj->setScale({ scale.x(), scale.y(), scale.z() });
	obj->setMarker(marker());

	std::vector<point3f> offset; offset.reserve(count);
	for (unsigned i = 0; i < count; i++)
		// NB: 考虑vbo复用，此处不作裁剪，否则只要坐标轴range变化就无法重用
		//if (curClipBox_ == -1 || clipBoxHistList_[curClipBox_].contains(pt)) // 预先裁剪
		offset.push_back(fn(i));

	obj->setInstPos(offset.data(), offset.size());
	obj->setInstColor(nullptr);
	obj->setInstSize(nullptr);

	pushRenderObject_(obj);
	return obj;
}


void* KcImOglPaint::drawMarkers(point_getter fn, color_getter clr, size_getter size, unsigned count)
{
	auto obj = (KcMarkerObject*)drawMarkers(fn, count);

	if (clr) {
		std::vector<color4f> clrs(count);
		for (unsigned i = 0; i < count; i++)
			clrs[i] = clr(i);
		obj->setInstColor(clrs.data());
	}

	if (size) {
		std::vector<float> sizes(count);
		for (unsigned i = 0; i < count; i++)
			sizes[i] = size(i);
		obj->setInstSize(sizes.data());
	}

	return obj;
}


void KcImOglPaint::drawLine(const point3& from, const point3& to)
{
	auto clr = clr_;
	auto width = lineWidth_;
	auto style = lineStyle_;
	auto pt0 = toNdc_(from);
	auto pt1 = toNdc_(to);

	auto drawFn = [clr, width, style, pt0, pt1]() {

		glColor4f(clr.r(), clr.g(), clr.b(), clr.a());
		glLineWidth(width);
		KuOglUtil::glLineStyle(style);
		
		glBegin(GL_LINES);
		glVertex3f(pt0.x(), pt0.y(), pt0.z());
		glVertex3f(pt1.x(), pt1.y(), pt1.z());
		glEnd();
	};

	currentRenderList().fns.push_back(drawFn);
}


void KcImOglPaint::drawRect(const point3& lower, const point3& upper)
{
	// NB: glRectf绘制的矩形并不完美，暂时调用基类的ImGui实现
	super_::drawRect(lower, upper);
	return;

	auto clr = clr_;
	auto width = lineWidth_;
	auto style = lineStyle_;
	auto pt0 = toNdc_(lower);
	auto pt1 = toNdc_(upper);

	auto drawFn = [clr, width, style, pt0, pt1]() {

		glColor4f(clr.r(), clr.g(), clr.b(), clr.a());
		glLineWidth(width);
		KuOglUtil::glLineStyle(style);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glRectf(pt0.x(), pt0.y(), pt1.x(), pt1.y());
	};

	currentRenderList().fns.push_back(drawFn);
}


void* KcImOglPaint::drawLineStrip(point_getter fn, unsigned count)
{
	auto obj = new KcLineObject(k_line_strip);

	auto decl = std::make_shared<KcVertexDeclaration>();
	decl->pushAttribute(KcVertexAttribute::k_float3, KcVertexAttribute::k_position);

	auto vbo = std::make_shared<KcGpuBuffer>();
	std::vector<point3f> vtx;
	vtx.reserve(count);
	for (unsigned i = 0; i < count; i++) // 装配数据
		vtx.push_back(fn(i));
	vbo->setData(vtx.data(), vtx.size() * sizeof(point3f), KcGpuBuffer::k_stream_draw);

	obj->pushVbo(vbo, decl);
	obj->setColor(clr_);
	obj->setWidth(lineWidth_);
	obj->setStyle(lineStyle_);
	pushRenderObject_(obj);

	return obj;
}


void* KcImOglPaint::drawLineStrips(const std::vector<point_getter>& fns, const std::vector<unsigned>& cnts)
{
	assert(fns.size() == cnts.size());

	auto obj = new KcLineObject(k_line_strip);

	auto decl = std::make_shared<KcVertexDeclaration>();
	decl->pushAttribute(KcVertexAttribute::k_float3, KcVertexAttribute::k_position);

	auto vbo = std::make_shared<KcGpuBuffer>();
	std::vector<point3f> vtx;
	unsigned total(0);
	for (auto c : cnts)
		total += c;
	total += cnts.size() - 1; // 每条线段之间插入nan
	vtx.reserve(total);

	for (unsigned i = 0; i < fns.size(); i++) {
		for (unsigned j = 0; j < cnts[i]; j++) // 装配数据
			vtx.push_back(fns[i](j));
		if (i != fns.size() - 1)
			vtx.push_back(point3f(KuMath::nan<float>()));
	}

	assert(vtx.size() == total);
	vbo->setData(vtx.data(), vtx.size() * sizeof(point3f), KcGpuBuffer::k_stream_draw);

	obj->pushVbo(vbo, decl);
	obj->setColor(clr_);
	obj->setWidth(lineWidth_);
	obj->setStyle(lineStyle_);
	pushRenderObject_(obj);

	return obj;
}


void* KcImOglPaint::fillBetween(point_getter fn1, point_getter fn2, unsigned count)
{
	// 构造vbo

	auto vbo = std::make_shared<KcGpuBuffer>();
	
	std::vector<float3> vtx;
	vtx.reserve((count - 1) * 6); // 每个区间绘制2个三角形，共6个顶点

	auto p00 = fn1(0);
	auto p01 = fn2(0);

	assert(p00.z() == p01.z()); // 要求各点都在一个z平面上

	for (unsigned i = 1; i < count; i++) {
		auto p10 = fn1(i);
		auto p11 = fn2(i);

		using point2 = KtPoint<float_t, 2>;
		KtLineS2d<float_t> ln0((const point2&)p00, (const point2&)p10);
		KtLineS2d<float_t> ln1((const point2&)p01, (const point2&)p11);
		auto pt = ln0.intersects(ln1);
		if (pt) { // 相交

			float3 ptm(pt->x(), pt->y(), p00.z());

			vtx.push_back(p01);
			vtx.push_back(p00);
			vtx.push_back(ptm);

			vtx.push_back(p10);
			vtx.push_back(p11);
			vtx.push_back(ptm);
		}
		else { // 不相交
			vtx.push_back(p01);
			vtx.push_back(p00);
			vtx.push_back(p10);

			vtx.push_back(p10);
			vtx.push_back(p11);
			vtx.push_back(p01);
		}

		p00 = p10, p01 = p11;
	}

	vbo->setData(vtx.data(), vtx.size() * sizeof(float3), KcGpuBuffer::k_stream_draw);

	auto obj = new KcRenderObject(k_triangles);

	auto decl = std::make_shared<KcVertexDeclaration>();
	decl->pushAttribute(KcVertexAttribute::k_float3, KcVertexAttribute::k_position);

	obj->pushVbo(vbo, decl);
	obj->setColor(clr_);
	pushRenderObject_(obj);

	return obj;
}


void KcImOglPaint::pushRenderObject_(KpRenderList_& rl, KcRenderObject* obj)
{
	//assert(obj->vbo() && obj->vertexDecl());
	
	switch (currentCoord())
	{
	case k_coord_local:
		obj->setProjMatrix(camera_.getMvpMat());
		break;

	case k_coord_world:
		obj->setProjMatrix(camera_.viewProjMatrix());
		break;
	
	case k_coord_screen:
		obj->setProjMatrix(camera_.getNsMatR_());
		break;

	case k_coord_ndc:
		obj->setProjMatrix(float4x4<>::identity());
		break;

	case k_coord_local_screen:
		assert(false);
		break;

	default:
		assert(false);
		break;
	}
	
	if (curClipBox_ != -1 && !inScreenCoord()) { // 屏幕坐标系不考虑clipBox
		auto& box = clipBoxHistList_[curClipBox_];
		obj->setClipBox({ box.lower(), box.upper() });
	}
	else {
		obj->setClipBox(KcRenderObject::aabb_t());
	}


	bool hasLight = lighting_ && obj->type() >= k_triangles; // 点线元素无须光照
	if (hasLight && !obj->hasNormal(false)) // 若无法线属性，自动生成法线
		pushNormals_(obj);
	obj->enableAttribute(KcVertexAttribute::k_normal, hasLight);
	if (hasLight) {
		obj->setNormalMatrix(camera_.getNormalMat());
		obj->setLightDir(lightDir_);
		obj->setLightColor(lightColor_);
		obj->setAmbientColor(ambientColor_);
	}

	if (obj->shader() == nullptr) { // 自动设置shader
		int type = KsShaderManager::k_mono;
		if (obj->hasColor(true)) type |= KsShaderManager::k_color;
		if (obj->hasUV(true)) type |= KsShaderManager::k_uv;
		if (obj->hasNormal(true)) type |= KsShaderManager::k_normal;
		if (obj->hasInst(true)) type |= KsShaderManager::k_instance;

		obj->setShader(KsShaderManager::singleton().fetchProg(type, flatShading_, curClipBox_ != -1));
	}

	rl.objs.emplace_back(obj);
}


KcRenderObject* KcImOglPaint::lastRenderObject_()
{
	return currentRenderList().objs.back().get();
}


void KcImOglPaint::drawText(const point3& anchor, const char* text, int align)
{
	drawText_(anchor, text, align, currentRenderList().texts, true);
}


void* KcImOglPaint::drawTexts(const std::vector<point3>& anchors, const std::vector<std::string>& texts, int align, const point2f& spacing)
{
	std::vector<point3f> ans; // 锚点
	std::vector<point4f> pos;
	std::vector<point4f> uvs;
	for (unsigned i = 0; i < texts.size(); i++) {
		auto rc = KuLayoutUtil::anchorAlignedRect({ 0, 0 }, textSize(texts[i].c_str()), align);
		point2f offset(-rc.upper());
		offset += spacing;

		pushTextData_(texts[i], pos, uvs);

		// 修正返回的偏移
		for (unsigned j = ans.size(); j < pos.size(); j++) {
			pos[j].x() += offset.x(), pos[j].y() += offset.y();
			pos[j].z() += offset.x(), pos[j].w() += offset.y();
		}

		ans.resize(pos.size(), anchors[i]);
	}
	
	int texId = (intptr_t)ImGui::GetWindowDrawList()->CmdBuffer.back().GetTexID();
	auto obj = new KcTextObject(texId);
	obj->setBufferData(ans.data(), pos.data(), uvs.data(), ans.size());

	auto scale = camera_.screenToNdc({ 1, 1, 1, 0 });
	obj->setScale({ scale.x(), scale.y(), scale.z() });
	obj->setColor(clr_);

	pushRenderObject_(obj);	
	return obj;
}


void KcImOglPaint::drawText_(const point3& anchor, const char* text, int align, std::vector<KpUvVbo>& vbo, bool normToNdc)
{
	auto szText = textSize(text);
	auto an = projectp(anchor); // 变换到屏幕坐标计算布局
	auto r = KuLayoutUtil::anchorAlignedRect({ an.x(), an.y() }, szText, align);
	auto topLeft = unprojectp(point2(r.lower().x(), r.lower().y()));
	topLeft.z() = anchor.z();
	drawText_(topLeft, unprojectv(point3(1, 0, 0)), unprojectv(point3(0, 1, 0)), text, vbo, normToNdc);
}


void KcImOglPaint::drawText(const point3& topLeft, const point3& hDir, const point3& vDir, const char* text)
{
	drawText_(topLeft, hDir, vDir, text, currentRenderList().texts, true);
}


void KcImOglPaint::pushTextData_(const std::string_view& text, std::vector<point4f>& pos, std::vector<point4f>& uvs) const
{
	point2f orig(0);
	auto font = ImGui::GetFont();
	auto s = text.data();
	auto eos = s + text.size();
	pos.reserve(pos.size() + text.size());
	uvs.reserve(uvs.size() + text.size());
	while (s < eos) {
		unsigned int c = (unsigned int)*s;
		if (c < 0x80) {
			s += 1;
		}
		else {
			s += ImTextCharFromUtf8(&c, s, eos);
			if (c == 0) // Malformed UTF-8?
				break;
		}

		if (c < 32) {
			if (c == '\n') {
				// TODO: 处理换行
				continue;
			}
			if (c == '\r')
				continue;
		}

		const ImFontGlyph* glyph = font->FindGlyph((ImWchar)c);
		if (glyph == nullptr)
			continue;

		if (glyph->Visible) {
			pos.emplace_back(orig.x() + glyph->X0, orig.y() + glyph->Y0, orig.x() + glyph->X1, orig.y() + glyph->Y1);
			uvs.emplace_back(glyph->U0, glyph->V0, glyph->U1, glyph->V1);
		}

		orig.x() += glyph->AdvanceX;
	}
}


void KcImOglPaint::drawText_(const point3& topLeft, const point3& hDir, const point3& vDir, const char* text, std::vector<KpUvVbo>& vbo, bool normToNdc)
{
	auto font = ImGui::GetFont();
	auto eos = text + strlen(text);

	auto hScale = 1.0 / projectv(hDir).length();
	auto vScale = 1.0 / projectv(vDir).length();
	auto height = vDir * font->FontSize * vScale; // 每行文字的高度. 暂时只支持单行渲染，该变量用不上

	auto s = text;
	auto orig = topLeft;
	vbo.reserve(vbo.size() + (eos - text) * 4);
	while (s < eos) {
		unsigned int c = (unsigned int)*s;
		if (c < 0x80) {
			s += 1;
		}
		else {
			s += ImTextCharFromUtf8(&c, s, eos);
			if (c == 0) // Malformed UTF-8?
				break;
		}

		if (c < 32) {
			if (c == '\n') {
				// TODO: 处理换行
				continue;
			}
			if (c == '\r')
				continue;
		}

		const ImFontGlyph* glyph = font->FindGlyph((ImWchar)c);
		if (glyph == nullptr)
			continue;

		if (glyph->Visible) {

			auto curPos = vbo.size();
			vbo.resize(curPos + 4); // 按quad图元绘制
			auto buf = vbo.data() + curPos;

			// 文字框的4个顶点对齐glyph
			auto dx1 = hDir * (hScale * glyph->X0);
			auto dy1 = vDir * (vScale * glyph->Y0);
			auto dx2 = hDir * (hScale * glyph->X1);
			auto dy2 = vDir * (vScale * glyph->Y1);

			if (normToNdc) {
				buf[0].pos = toNdc_(orig + dx1 + dy1); // top-left
				buf[1].pos = toNdc_(orig + dx2 + dy1); // top-right
				buf[2].pos = toNdc_(orig + dx2 + dy2); // bottom-right
				buf[3].pos = toNdc_(orig + dx1 + dy2); // bottom-left
			}
			else {
				buf[0].pos = (orig + dx1 + dy1); // top-left
				buf[1].pos = (orig + dx2 + dy1); // top-right
				buf[2].pos = (orig + dx2 + dy2); // bottom-right
				buf[3].pos = (orig + dx1 + dy2); // bottom-left
			}

			// 文字框的纹理坐标
			float u1 = glyph->U0;
			float v1 = glyph->V0;
			float u2 = glyph->U1;
			float v2 = glyph->V1;

			buf[0].uv = { u1, v1 };
			buf[1].uv = { u2, v1 };
			buf[2].uv = { u2, v2 };
			buf[3].uv = { u1, v2 };

			for (int i = 0; i < 4; i++)
				buf[i].clr = clr_;
		}

		orig += hDir * glyph->AdvanceX * hScale;
	}
}


KcRenderObject* KcImOglPaint::makeTextVbo_(std::vector<KpUvVbo>& text)
{
	if (!text.empty()) {
		auto obj = new KcRenderObject(k_quads);
		obj->setShader(KsShaderManager::singleton().progColorUV(true, curClipBox_ != -1)); // 目前文字渲染始终使用flat模式

		auto decl = std::make_shared<KcVertexDeclaration>();
		decl->pushAttribute(KcVertexAttribute::k_float3, KcVertexAttribute::k_position);
		decl->pushAttribute(KcVertexAttribute::k_float2, KcVertexAttribute::k_texcoord);
		decl->pushAttribute(KcVertexAttribute::k_float4, KcVertexAttribute::k_diffuse);
		assert(decl->vertexSize() == sizeof(text[0]));

		auto vbo = std::make_shared<KcGpuBuffer>();
		vbo->setData(text.data(), text.size() * sizeof(text[0]), KcGpuBuffer::k_stream_draw);

		obj->pushVbo(vbo, decl);
		return obj;
	}

	return nullptr;
}


void KcImOglPaint::pushTextVbo_(KpRenderList_& rl)
{
	if (!rl.texts.empty()) {
		auto obj = makeTextVbo_(rl.texts);
		pushCoord(k_coord_ndc); // 全局text使用ndc坐标，不须在shader中进行坐标变换
		pushRenderObject_(rl, obj);
		popCoord();
	}
}


void KcImOglPaint::pushColorVbo_(KpRenderList_& rl)
{
	if (!rl.tris.empty()) {
		auto obj = new KcRenderObject(k_triangles);
		obj->setShader(KsShaderManager::singleton().progColor(false, false));

		auto decl = std::make_shared<KcVertexDeclaration>();
		decl->pushAttribute(KcVertexAttribute::k_float3, KcVertexAttribute::k_position);
		decl->pushAttribute(KcVertexAttribute::k_float4, KcVertexAttribute::k_diffuse);

		auto vbo = std::make_shared<KcGpuBuffer>();
		vbo->setData(rl.tris.data(), rl.tris.size() * sizeof(KpColorVbo_), KcGpuBuffer::k_stream_draw);
		obj->pushVbo(vbo, decl);

		pushCoord(k_coord_screen); // 确保pushRenderObject_压入屏幕坐标变换阵
		// 自带color，不许设置obj的颜色值
		pushRenderObject_(rl, obj);
		popCoord();
	}
}


void* KcImOglPaint::drawGeom(vtx_decl_ptr decl, geom_ptr geom)
{
	assert(geom->vertexSize() == decl->vertexSize());

	KcRenderObject* obj = nullptr;
	bool hasNormal = decl->hasNormal();
	bool hasColor = decl->hasColor();

	if (geom->type() == k_points) {
		auto pointObj = new KcPointObject;
		pointObj->setSize(markerSize_);
	}
	else if (geom->type() == k_lines 
		|| geom->type() == k_line_strip 
		|| geom->type() == k_line_loop) {
		auto lineObj = new KcLineObject(geom->type());
		lineObj->setWidth(lineWidth_);
		lineObj->setStyle(lineStyle_);
		obj = lineObj;
	}
	else {
		KcEdgedObject* edgedObj = new KcEdgedObject(geom->type());
		edgedObj->setEdgeWidth(lineWidth_);
		edgedObj->setEdgeStyle(lineStyle_);
		edgedObj->setFilled(filled_); edgedObj->setEdged(edged_);
		edgedObj->setEdgeColor(secondaryClr_);
		edgedObj->setEdgeShader(KsShaderManager::singleton().progMono(curClipBox_ != -1));
		obj = edgedObj;
	}

	if (hasNormal) {
		// TODO: ((KcLightenObject*)obj)->setNormalMatrix(camera_.getNormalMatrix());
	}

	auto vbo = std::make_shared<KcGpuBuffer>();
	vbo->setData(geom->vertexData(), geom->vertexCount() * geom->vertexSize(), KcGpuBuffer::k_stream_draw);

	obj->pushVbo(vbo, decl);
	if (geom->indexCount() > 0) {
		auto ibo = std::make_shared<KcGpuBuffer>(KcGpuBuffer::k_index_buffer);
		ibo->setData(geom->indexData(), geom->indexCount() * geom->indexSize(), KcGpuBuffer::k_stream_draw);
		obj->pushIbo(ibo, geom->indexCount());
	}

	obj->setColor(clr_);
	pushRenderObject_(obj);

	return obj;
}


void KcImOglPaint::setViewport(const rect_t& vp)
{
	auto pos = std::find(viewportHistList_.cbegin(), viewportHistList_.cend(), vp);
	if (pos == viewportHistList_.cend()) {
		curViewport_ = viewportHistList_.size();
		viewportHistList_.push_back(vp);
	}
	else {
		curViewport_ = std::distance(viewportHistList_.cbegin(), pos);
	}

	assert(viewportHistList_[curViewport_] == vp);

	super_::setViewport(vp);
}


void KcImOglPaint::pushClipRect(const rect_t& cr)
{
	auto pos = std::find(clipRectHistList_.cbegin(), clipRectHistList_.cend(), cr);
	if (pos == clipRectHistList_.cend()) {
		clipRectStack_.push_back(clipRectHistList_.size());
		clipRectHistList_.push_back(cr);
	}
	else {
		clipRectStack_.push_back(std::distance(clipRectHistList_.cbegin(), pos));
	}

	super_::pushClipRect(cr); // TODO: 当完全不用ImGui绘制时，可删除此代码
}


void KcImOglPaint::popClipRect()
{
	clipRectStack_.pop_back();

	super_::popClipRect(); // TODO: 当完全不用ImGui绘制时，可删除此代码
}


void KcImOglPaint::enableClipBox(point3 lower, point3 upper)
{
	auto pos = std::find(clipBoxHistList_.cbegin(), clipBoxHistList_.cend(), aabb_t(lower, upper));
	if (pos == clipBoxHistList_.cend()) {
		curClipBox_ = clipBoxHistList_.size();
		clipBoxHistList_.emplace_back(lower, upper);
	}
	else {
		curClipBox_ = std::distance(clipBoxHistList_.cbegin(), pos);
	}
}


void KcImOglPaint::disableClipBox()
{
	curClipBox_ = -1;
}


KcImOglPaint::KpRenderList_& KcImOglPaint::currentRenderList()
{
	auto curClipRect = clipRectStack_.empty() ? -1 : clipRectStack_.back();
	return renderList_[kRenderState_(curViewport_, curClipRect, curClipBox_, polygonOffset_)];
}


void KcImOglPaint::configOglState_()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (glProvokingVertex)
		glProvokingVertex(GL_LAST_VERTEX_CONVENTION); // 使用最后一个顶点数据作为flat着色模式下整个面片的属性

	if (antialiasing()) {
		glEnable(GL_POINT_SMOOTH);
		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_POLYGON_SMOOTH);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		glDisable(GL_POINT_SMOOTH);
		glDisable(GL_LINE_SMOOTH);
		glDisable(GL_POLYGON_SMOOTH);
		glDisable(GL_MULTISAMPLE); 

		// 以下设置能提升文字渲染的清晰度（避免插值）
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	if (depthTest()) 
		glEnable(GL_DEPTH_TEST);
	else 
		glDisable(GL_DEPTH_TEST);

	//glFrontFace(GL_CW); // 定义顺时针为正面
	//glEnable(GL_CULL_FACE); // for debug
	//glCullFace(GL_FRONT);
}


void KcImOglPaint::drawRenderList_()
{
	configOglState_();

	glDisable(GL_POLYGON_OFFSET_FILL);
	bool polygonOffset(false);
	unsigned viewport(-1), clipRect(-1), clipBox(-2); // NB: clipBox可以等于-1，所以此处初始化为-2，表示未赋值

	// NB: 逆序，否则坐标轴会被plottables覆盖
	for (auto rd = renderList_.rbegin(); rd != renderList_.rend(); rd++) { 
		auto& state = rd->first;
		if (std::get<0>(state) != viewport) {
			viewport = std::get<0>(state);
			glViewport_(viewport);
			camera_.setViewport(viewportHistList_[viewport]); // 更新vp相关变换阵
		}
		if (std::get<1>(state) != clipRect) {
			clipRect = std::get<1>(state);
			glScissor_(clipRect);
		}
		if (std::get<2>(state) != clipBox) {
			clipBox = std::get<2>(state);
			glClipPlane_(clipBox);
		}
		if (std::get<3>(state) != polygonOffset) {
			polygonOffset = std::get<3>(state);
			if (polygonOffset) {
				glPolygonOffset(1, 1);
				glEnable(GL_POLYGON_OFFSET_FILL);
			}
			else {
				glDisable(GL_POLYGON_OFFSET_FILL);
			}
		}

		auto& rl = rd->second;
		pushTextVbo_(rl);
		pushColorVbo_(rl);

		for (auto& i : rl.objs) i->draw();

		// NB: 小件元素最后绘制
		// TODO: 在该模式下，没有plot2d所需要的layer概念，只能通过深度测试来模拟（须设置正确的z值）
		if (!rl.fns.empty())
			KcGlslProgram::useProgram(0); // 禁用shader，fns须enable固定管线
		for (auto& i : rl.fns) i();
	}

	// NB: ImGui不会恢复多实例状态，此处重置，否则会影响ImGui渲染结果（比如combox下拉框无文字）
	for (unsigned i = 0; i < 8; i++)
		glVertexAttribDivisor(i, 0);
}


void KcImOglPaint::glViewport_(unsigned id)
{
	assert(id != -1);

	assert(id < viewportHistList_.size());
	auto& rc = viewportHistList_[id];
	auto y0 = ImGui::GetMainViewport()->Size.y - rc.upper().y();
	glViewport(rc.lower().x(), y0, rc.width(), rc.height());
}


void KcImOglPaint::glScissor_(unsigned id)
{
	assert(id != -1);

	assert(id < clipRectHistList_.size());
	auto& rc = clipRectHistList_[id];
	auto y0 = ImGui::GetMainViewport()->Size.y - rc.upper().y();
	glScissor(rc.lower().x(), y0 + 1, rc.width() + 1, rc.height()); // 下方多1个像素，右方少1个像素，在此修正
}


void KcImOglPaint::glClipPlane_(unsigned id)
{
	static constexpr unsigned planes[] = {
		GL_CLIP_PLANE0,
		GL_CLIP_PLANE1,
		GL_CLIP_PLANE2,
		GL_CLIP_PLANE3,
		GL_CLIP_PLANE4,
		GL_CLIP_PLANE5
	};

	if (id != -1) {
		assert(id < clipBoxHistList_.size());
		auto& aabb = clipBoxHistList_[id];
		GLdouble clipPlane[4] = { 0 };
		for (int i = 0; i < 3; i++) {
			clipPlane[i] = 1; clipPlane[3] = -aabb.lower()[i];
			glClipPlane(planes[2 * i], clipPlane);
			glEnable(planes[2 * i]);

			clipPlane[i] = -1; clipPlane[3] = aabb.upper()[i];
			glClipPlane(planes[2 * i + 1], clipPlane);
			glEnable(planes[2 * i + 1]);

			clipPlane[i] = 0;
		}
	}
	else {
		for(unsigned i = 0; i < std::size(planes); i++)
		    glDisable(planes[i]);
	}
}


void KcImOglPaint::grab(int x, int y, int width, int height, void* data)
{
	y = ImGui::GetMainViewport()->Size.y - y - height;
	glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
}


KcImOglPaint::KpColorVbo_* KcImOglPaint::newColorVbo_(unsigned tris)
{
	auto& rl = currentRenderList();
	auto pos = rl.tris.size();
	rl.tris.resize(pos + tris);
	return rl.tris.data() + pos;
}


void KcImOglPaint::pushTrisSoild_(const float3 pos[], unsigned c, const float4& clr)
{
	auto vbo = newColorVbo_(c);
	for (unsigned i = 0; i < c; i++) {
		vbo->pos = *pos++;
		vbo->clr = clr;
		++vbo;
	}
}


void KcImOglPaint::pushTrisSoild_(const point2 pos[], unsigned c, const float4& clr)
{
	auto vbo = newColorVbo_(c);
	for (unsigned i = 0; i < c; i++) {
		vbo->pos = { pos->x(), pos->y(), 0 };
		vbo->clr = clr;
		++pos, ++vbo;
	}
}


void* KcImOglPaint::redraw(void* obj)
{
	auto iter = savedObjList_.find(obj);
	if (iter != savedObjList_.end()) {
		auto newObj = iter->second->clone();
		syncObjProps_(newObj); // 同步当前渲染属性
		pushRenderObject_(newObj); // 重置全局渲染状态，并压入待渲染队列
		return newObj;
	}

	return nullptr;
}


void KcImOglPaint::syncObjProps_(KcRenderObject* obj)
{
	if (dynamic_cast<KcEdgedObject*>(obj)) {
		auto eo = dynamic_cast<KcEdgedObject*>(obj);
		eo->setEdgeWidth(lineWidth_);
		eo->setEdgeStyle(lineStyle_);
		eo->setFilled(filled_); eo->setEdged(edged_);
		eo->setEdgeColor(secondaryClr_);
	}
	else if (dynamic_cast<KcPointObject*>(obj)) {
		auto po = dynamic_cast<KcPointObject*>(obj);
		po->setSize(markerSize_);
	}
	else if (dynamic_cast<KcLineObject*>(obj)) {
		auto lo = dynamic_cast<KcLineObject*>(obj);
		lo->setWidth(lineWidth_);
		lo->setStyle(lineStyle_);
	}
	else if (dynamic_cast<KcMarkerObject*>(obj)) {
		auto mo = dynamic_cast<KcMarkerObject*>(obj);
		auto scale = camera_.screenToNdc({ 1, 1, 1, 0 });
		//assert(KuMath::almostEqual(scale.length(), 1.));
		mo->setScale({ scale.x(), scale.y(), scale.z() });
		mo->setMarker(marker());
	}
	else if (dynamic_cast<KcTextObject*>(obj)) {
		auto to = dynamic_cast<KcTextObject*>(obj);
		auto scale = camera_.screenToNdc({ 1, 1, 1, 0 });
		to->setScale({ scale.x(), scale.y(), scale.z() });
		// TODO: 文本设置
		//mo->setMarker(marker());
	}

	obj->setColor(clr_);

	// 在此置shader为空，由pushRenderObject设定shader，以同步flat渲染状态
    obj->setShader(nullptr);
}


void KcImOglPaint::pushNormals_(KcRenderObject* obj)
{
	// TODO: 暂时假定第1个元素为position
	assert(obj->vertexDecl(0)->getAttribute(0).semantic() == KcVertexAttribute::k_position);
	auto vbo = obj->vbo(0);
	auto vtxSize = obj->vertexDecl(0)->vertexSize();
	auto vboSize = vbo->bytesCount() / vtxSize;

	std::vector<vec3f> normals(vboSize);

	if (obj->iboCount() == 0) {
		auto vtx = (const point3f*)vbo->map(KcGpuBuffer::k_read_only);

		if (obj->type() == k_triangles) {
			KuMesh::triNormalsFlat(vtx, normals.data(), vboSize, vtxSize);
		}
		else if (obj->type() == k_quads) {
			KuMesh::quadNormalsFlat(vtx, normals.data(), vboSize, vtxSize);
		}
		else {
			assert(false);
		}

		vbo->unmap();
	}
	else {
		assert(obj->iboCount() == 1); // 只能处理单个索引对象的情况
		auto ibo = obj->ibo(0);
		auto iboSize = obj->iboSize(0);
		auto idxSize = ibo->bytesCount() / iboSize;
		assert(idxSize == 4);

		auto vtx = (const point3f*)vbo->map(KcGpuBuffer::k_read_only);
		auto idx = (const std::uint32_t*)ibo->map(KcGpuBuffer::k_read_only);

		if (obj->type() == k_triangles) {
			KuMesh::triNormalsAve(idx, iboSize, vtx, normals.data(), vboSize, vtxSize);
		}
		else if (obj->type() == k_quads) {
			KuMesh::quadNormalsAve(idx, iboSize, vtx, normals.data(), vboSize, vtxSize);
		}
		else {
			assert(false);
		}

		ibo->unmap();
		vbo->unmap();
	}

	auto nvbo = std::make_shared<KcGpuBuffer>();
	nvbo->setData(normals.data(), normals.size() * sizeof(normals[0]), KcGpuBuffer::k_dynamic_draw);
	auto decl = std::make_shared<KcVertexDeclaration>();
	decl->pushAttribute(KcVertexAttribute(obj->vertexDecl(0)->attributeCount(),
		KcVertexAttribute::k_float3, 0, KcVertexAttribute::k_normal));
	obj->pushVbo(nvbo, decl);
}
