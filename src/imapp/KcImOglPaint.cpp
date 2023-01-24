#include "KcImOglPaint.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "glad.h"
#include "KcVertexDeclaration.h"
#include "KtLineS2d.h"
#include "KtuMath.h"
#include "opengl/KcGlslProgram.h"
#include "opengl/KcGlslShader.h"
#include "opengl/KcGpuBuffer.h"
#include "opengl/KcPointObject.h"
#include "opengl/KcLineObject.h"
#include "opengl/KcEdgedObject.h"
#include "opengl/KcLightenObject.h"
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


void KcImOglPaint::beginPaint()
{
	renderList_.clear();

	viewportHistList_.clear();
	clipRectHistList_.clear();
	
	viewportHistList_.push_back(viewport());
	curViewport_ = 0;

	auto crmin = ImGui::GetWindowDrawList()->GetClipRectMin();
	auto crmax = ImGui::GetWindowDrawList()->GetClipRectMax();
	clipRectHistList_.emplace_back(point2(crmin.x, crmin.y), point2(crmax.x, crmax.y));
	clipRectStack_.push_back(0);
	
	clipBoxHistList_.clear();
	curClipBox_ = -1;

	super_::beginPaint();
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
		auto p = camera_.localToNdc(pt);
		return { p.x(), p.y(), p.z() };
	}

	case k_coord_world:
	{
		auto p = camera_.worldToNdc(pt);
		return { p.x(), p.y(), p.z() };
	}

	case k_coord_screen:
	{
		auto p = camera_.screenToNdc(pt);
		return { p.x(), p.y(), p.z() };
	}

	case k_coord_local_screen:
	{
		auto p = camera_.localToWorld(pt);
		p = camera_.screenToNdc(p);
		return { p.x(), p.y(), p.z() };
	}

	}

	assert(false);
	return point3(0);
}


void KcImOglPaint::drawMarker(const point3& pt, bool outline)
{
	super_::drawMarker(pt, outline);
}


void KcImOglPaint::drawPoints_(point_getter1 fn, unsigned count)
{
	auto obj = new KcPointObject;

	auto decl = std::make_shared<KcVertexDeclaration>();
	decl->pushAttribute(KcVertexAttribute::k_float3, KcVertexAttribute::k_position);

	auto vbo = std::make_shared<KcGpuBuffer>();
	std::vector<point3f> vtx;
	for (unsigned i = 0; i < count; i++) // 装配数据
		vtx.push_back(fn(i));
	vbo->setData(vtx.data(), vtx.size() * sizeof(point3f), KcGpuBuffer::k_stream_draw);

	obj->setVBO(vbo, decl);
	obj->setSize(markerSize_);
	pushRenderObject_(obj);
}


void KcImOglPaint::drawCircles_(point_getter1 fn, unsigned count, bool outline)
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
	drawGeom(decl, geom, true, false);

	if (outline) { // 描边
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

		auto oldClr = clr_;
		clr_ = secondaryClr_;
		pushRenderObject_(edgeObj);
		clr_ = oldClr;
	}

	popCoord();
}


namespace kPrivate
{
	template<int N, bool forceLines = false>
	void drawPolyMarkers_(KvPaint& paint, KvPaint::point_getter1 fn, unsigned count, 
		const KvPaint::point2 poly[N], float markerSize, bool outline)
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
		paint.drawGeomSolid(geom, true, outline);
		paint.popCoord();
	}
}


void KcImOglPaint::addMarkers_(point_getter1 fn, unsigned count, const point2* fillVtx, unsigned numFill,
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


void KcImOglPaint::addConvexPolyFilled_(point_getter1 fn, unsigned count, const float4& clr)
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


void KcImOglPaint::addLineLoop_(point_getter1 fn, unsigned count, const float4& clr)
{
	for (unsigned j = 1; j < count; j++)
		addLine_(fn(j - 1), fn(j), clr);
	addLine_(fn(count - 1), fn(0), clr);
}


void KcImOglPaint::addMarkers_(point_getter1 fn, unsigned count, const point2* vtxBuf, unsigned numVtx, bool outline)
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

		if (outline)
			addLineLoop_(fns, numVtx, secondaryClr_);
	}
}


void KcImOglPaint::drawQuadMarkers_(point_getter1 fn, unsigned count, const point2 quad[4], bool outline)
{
	kPrivate::drawPolyMarkers_<4>(*this, fn, count, quad, markerSize_, outline);
}


void KcImOglPaint::drawTriMarkers_(point_getter1 fn, unsigned count, const point2 tri[3], bool outline)
{
	kPrivate::drawPolyMarkers_<3>(*this, fn, count, tri, markerSize_, outline);
}


void KcImOglPaint::drawMarkers(point_getter1 fn, unsigned count, bool outline)
{
	static const double SQRT_2_2 = std::sqrt(2.) / 2.;
	static const double SQRT_3_2 = std::sqrt(3.) / 2.;

	switch (markerType_)
	{
	case KpMarker::k_dot:
		drawPoints_(fn, count);
		return;

	case KpMarker::k_cross:
	{
		static const point2 cross[4] = { 
			point2(-SQRT_2_2,-SQRT_2_2),
			point2(SQRT_2_2,SQRT_2_2),
			point2(SQRT_2_2,-SQRT_2_2),
			point2(-SQRT_2_2,SQRT_2_2) 
		};
		kPrivate::drawPolyMarkers_<4, true>(*this, fn, count, cross, markerSize_, outline);
	}
	    return;

	case KpMarker::k_plus:
	{
		static const point2 plus[4] = { point2(-1, 0), point2(1, 0), point2(0, -1), point2(0, 1) };
		kPrivate::drawPolyMarkers_<4, true>(*this, fn, count, plus, markerSize_, outline);
	}
	    return;

	case KpMarker::k_asterisk:
	{
		static const point2 asterisk[6] = { 
			point2(-SQRT_3_2, -0.5f), 
			point2(SQRT_3_2, 0.5f),  
			point2(-SQRT_3_2, 0.5f),
			point2(SQRT_3_2, -0.5f),
			point2(0, -1), 
			point2(0, 1) 
		};
		kPrivate::drawPolyMarkers_<6>(*this, fn, count, asterisk, markerSize_, outline);
	}
		return;

	case KpMarker::k_square:
	case KpMarker::k_diamond:
	case KpMarker::k_left:
	case KpMarker::k_right:
	case KpMarker::k_up:
	case KpMarker::k_down:
	default:
		break;
	};

	// 带outline的marker赞使用ImGui实现(issue I6B5ES)
	super_::drawMarkers(fn, count, outline);
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


void KcImOglPaint::drawLineStrip(point_getter1 fn, unsigned count)
{
	auto obj = new KcLineObject(k_line_strip);

	auto decl = std::make_shared<KcVertexDeclaration>();
	decl->pushAttribute(KcVertexAttribute::k_float3, KcVertexAttribute::k_position);

	auto vbo = std::make_shared<KcGpuBuffer>();
	std::vector<point3f> vtx;
	for (unsigned i = 0; i < count; i++) // 装配数据
		vtx.push_back(fn(i));
	vbo->setData(vtx.data(), vtx.size() * sizeof(point3f), KcGpuBuffer::k_stream_draw);

	obj->setVBO(vbo, decl);
	obj->setWidth(lineWidth_);
	obj->setStyle(lineStyle_);
	pushRenderObject_(obj);
}


void KcImOglPaint::fillBetween(point_getter1 fn1, point_getter1 fn2, unsigned count)
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

	obj->setVBO(vbo, decl);
	pushRenderObject_(obj);
}


void KcImOglPaint::pushRenderObject_(KpRenderList_& rl, KcRenderObject* obj)
{
	assert(obj->vbo() && obj->vertexDecl());
	
	obj->setColor(clr_);
	switch (currentCoord())
	{
	case k_coord_local:
		obj->setProjMatrix(camera_.getMvpMat());
		break;

	case k_coord_world:
		obj->setProjMatrix(camera_.getVpMatrix());
		break;
	
	case k_coord_screen:
		obj->setProjMatrix(camera_.getNsMatR_());
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


	if (obj->shader() == nullptr) { // 自动设置shader
		if (!obj->vertexDecl()->hasColor())
			obj->setShader(KsShaderManager::singleton().progMono());
		else
			obj->setShader(KsShaderManager::singleton().progColor(flatShading()));
	}

	rl.objs.emplace_back(obj);
}


KcRenderObject* KcImOglPaint::lastRenderObject_()
{
	return currentRenderList().objs.back().get();
}


void KcImOglPaint::drawText(const point3& anchor, const char* text, int align)
{
	auto szText = textSize(text);
	auto an = projectp(anchor); // 变换到屏幕坐标计算布局
	auto r = KuLayoutUtil::anchorAlignedRect({ an.x(), an.y() }, szText, align);
	auto topLeft = unprojectp(point2(r.lower().x(), r.lower().y())); 
	topLeft.z() = anchor.z();
	drawText(topLeft, unprojectv(point3(1, 0, 0)), unprojectv(point3(0, 1, 0)), text);
}


void KcImOglPaint::drawText(const point3& topLeft, const point3& hDir, const point3& vDir, const char* text)
{
	auto font = ImGui::GetFont();
	auto eos = text + strlen(text);

	auto hScale = 1.0 / projectv(hDir).length();
	auto vScale = 1.0 / projectv(vDir).length();
	auto height = vDir * font->FontSize * vScale; // 每行文字的高度. 暂时只支持单行渲染，该变量用不上

	auto s = text;
	auto orig = topLeft;
	auto& texts = currentRenderList().texts;
	texts.reserve(texts.size() + (eos - text) * 4);
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

			auto curPos = texts.size();
			texts.resize(curPos + 4); // 按quad图元绘制
			auto buf = texts.data() + curPos;

			// 文字框的4个顶点对齐glyph
			auto dx1 = hDir * (hScale * glyph->X0);
			auto dy1 = vDir * (vScale * glyph->Y0);
			auto dx2 = hDir * (hScale * glyph->X1);
			auto dy2 = vDir * (vScale * glyph->Y1);

			buf[0].pos = toNdc_(orig + dx1 + dy1); // top-left
			buf[1].pos = toNdc_(orig + dx2 + dy1); // top-right
			buf[2].pos = toNdc_(orig + dx2 + dy2); // bottom-right
			buf[3].pos = toNdc_(orig + dx1 + dy2); // bottom-left

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


void KcImOglPaint::pushTextVbo_(KpRenderList_& rl)
{
	if (!rl.texts.empty()) {
		auto obj = new KcRenderObject(k_quads);
		obj->setShader(KsShaderManager::singleton().progColorUV(true)); // 文字渲染始终使用flat模式

		auto decl = std::make_shared<KcVertexDeclaration>();
		decl->pushAttribute(KcVertexAttribute::k_float3, KcVertexAttribute::k_position);
		decl->pushAttribute(KcVertexAttribute::k_float2, KcVertexAttribute::k_texcoord);
		decl->pushAttribute(KcVertexAttribute::k_float4, KcVertexAttribute::k_diffuse);
		assert(decl->vertexSize() == sizeof(rl.texts[0]));

		auto vbo = std::make_shared<KcGpuBuffer>();
		vbo->setData(rl.texts.data(), rl.texts.size() * sizeof(rl.texts[0]), KcGpuBuffer::k_stream_draw);

		obj->setVBO(vbo, decl);
		obj->setProjMatrix(float4x4<>::identity());
		rl.objs.emplace_back(obj);
	}
}


void KcImOglPaint::pushColorVbo_(KpRenderList_& rl)
{
	if (!rl.tris.empty()) {
		auto obj = new KcRenderObject(k_triangles);
		obj->setShader(KsShaderManager::singleton().progColor(false));

		auto decl = std::make_shared<KcVertexDeclaration>();
		decl->pushAttribute(KcVertexAttribute::k_float3, KcVertexAttribute::k_position);
		decl->pushAttribute(KcVertexAttribute::k_float4, KcVertexAttribute::k_diffuse);

		auto vbo = std::make_shared<KcGpuBuffer>();
		vbo->setData(rl.tris.data(), rl.tris.size() * sizeof(KpColorVbo_), KcGpuBuffer::k_stream_draw);
		obj->setVBO(vbo, decl);

		pushCoord(k_coord_screen); // 确保pushRenderObject_压入屏幕坐标变换阵
		pushRenderObject_(rl, obj);
		popCoord();
	}
}


void KcImOglPaint::drawGeom(vtx_decl_ptr decl, geom_ptr geom, bool fill, bool showEdge)
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
		edgedObj->setFilled(fill); edgedObj->setEdged(showEdge);
		if (showEdge && !hasColor)
			edgedObj->setEdgeColor(secondaryClr_);
		obj = edgedObj;
	}

	if (hasNormal) {
		// TODO: ((KcLightenObject*)obj)->setNormalMatrix(camera_.getNormalMatrix());
	}

	auto vbo = std::make_shared<KcGpuBuffer>();
	vbo->setData(geom->vertexData(), geom->vertexCount() * geom->vertexSize(), KcGpuBuffer::k_stream_draw);

	obj->setVBO(vbo, decl);
	if (geom->indexCount() > 0) {
		auto ibo = std::make_shared<KcGpuBuffer>(KcGpuBuffer::k_index_buffer);
		ibo->setData(geom->indexData(), geom->indexCount() * geom->indexSize(), KcGpuBuffer::k_stream_draw);
		obj->setIBO(ibo, geom->indexCount());
	}

	pushRenderObject_(obj);
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
}


void KcImOglPaint::popClipRect()
{
	clipRectStack_.pop_back();
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
	return renderList_[kRenderState_(curViewport_, curClipRect, curClipBox_)];
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
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else {
		glDisable(GL_POINT_SMOOTH);
		glDisable(GL_LINE_SMOOTH);
		glDisable(GL_POLYGON_SMOOTH);
	}

	if (depthTest()) {
		glEnable(GL_DEPTH_TEST);
		glClear(GL_DEPTH_BUFFER_BIT);
	}
	else {
		glDisable(GL_DEPTH_TEST);
	}
}


void KcImOglPaint::drawRenderList_()
{
	configOglState_();

	unsigned viewport(-1), clipRect(0), clipBox(-2); // NB: clipBox可以等于-1，所以此处初始化为-2，表示未赋值
	for (auto& rd : renderList_) {
		auto& state = rd.first;
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

		auto& rl = rd.second;
		pushTextVbo_(rl);
		pushColorVbo_(rl);

		for (auto& i : rl.objs) i->draw();

		// NB: 小件元素最后绘制
		// TODO: 在该模式下，没有plot2d所需要的layer概念，只能通过深度测试来模拟（须设置正确的z值）
		if (!rl.fns.empty())
			KcGlslProgram::useProgram(0); // 禁用shader，fns须enable固定管线
		for (auto& i : rl.fns) i();
	}
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
	glScissor(rc.lower().x(), y0, rc.width(), rc.height());
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
