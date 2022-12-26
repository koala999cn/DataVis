#include "KcImOglPaint.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "glad.h"
#include "KcVertexDeclaration.h"
#include "opengl/KcGlslProgram.h"
#include "opengl/KcGlslShader.h"
#include "opengl/KcGpuBuffer.h"
#include "opengl/KcPointObject.h"
#include "opengl/KcLineObject.h"
#include "opengl/KcLightenObject.h"
#include "opengl/KsShaderManager.h"


namespace kPrivate
{
	// color和width已设置好，此处主要设置style(TODO)
	void oglLine(int style, const KcImOglPaint::point3& from, const KcImOglPaint::point3& to)
	{
		glBegin(GL_LINES);
		glVertex3f(from.x(), from.y(), from.z());
		glVertex3f(to.x(), to.y(), to.z());
		glEnd();
	}

	void oglDrawRenderList(const ImDrawList*, const ImDrawCmd* cmd)
	{
		auto paint = (KcImOglPaint*)cmd->UserCallbackData;
		paint->drawRenderList_();
	}
}


KcImOglPaint::KcImOglPaint(camera_type& cam)
	: super_(cam)
{
	curViewport_ = -1;
	curClipBox_ = -1;
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
	auto p = camera_.localToNdc(pt);

	// opengl固定管线默认NDC是左手系，p的结果是右手系，所以需要给z值取反???
	return { p.x(), p.y(), p.z() };
}


void KcImOglPaint::drawPoint(const point3& pt)
{
	auto clr = clr_;
	auto ptSize = pointSize_;
	auto p = toNdc_(pt);

	auto drawFn = [clr, ptSize, p]() {

		glColor4f(clr.r(), clr.g(), clr.b(), clr.a());
		glPointSize(ptSize);
		glEnable(GL_POINT_SMOOTH);
		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBegin(GL_POINTS);
		glVertex3f(p.x(), p.y(), p.z());
		glEnd();

	};

	currentRenderList().fns.push_back(drawFn);
}


void KcImOglPaint::drawPoints(point_getter fn, unsigned count)
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
	obj->setColor(clr_);
	obj->setSize(pointSize_);
	obj->setProjMatrix(camera_.getMvpMat());
	if (curClipBox_ != -1) {
		auto& box = clipBoxHistList_[curClipBox_];
		obj->setClipBox({ box.lower(), box.upper() });
	}

	currentRenderList().objs.emplace_back(obj);
}


void KcImOglPaint::drawLine(const point3& from, const point3& to)
{
	auto clr = clr_;
	auto lnWidth = lineWidth_;
	auto style = lineStyle_;
	auto pt0 = toNdc_(from);
	auto pt1 = toNdc_(to);

	auto drawFn = [clr, lnWidth, pt0, pt1, style]() {

		glLineWidth(lnWidth);
		glColor4f(clr.r(), clr.g(), clr.b(), clr.a());
		//glEnable(GL_LINE_SMOOTH);
		//glHint(GL_LINE_SMOOTH, GL_NICEST);
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		kPrivate::oglLine(style, pt0, pt1);

	};

	currentRenderList().fns.push_back(drawFn);
}


void KcImOglPaint::drawLineStrip(point_getter fn, unsigned count)
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
	obj->setColor(clr_);
	obj->setWidth(lineWidth_);
	obj->setProjMatrix(camera_.getMvpMat());
	if (curClipBox_ != -1) {
		auto& box = clipBoxHistList_[curClipBox_];
		obj->setClipBox({ box.lower(), box.upper() });
	}

	currentRenderList().objs.emplace_back(obj);
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
			TextVbo* buf = texts.data() + curPos;

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
		obj->setShader(KsShaderManager::singleton().programColorUV());

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


void KcImOglPaint::drawGeom(vtx_decl_ptr decl, geom_ptr geom)
{
	assert(geom->vertexSize() == decl->vertexSize());

	bool hasNormal = decl->hasNormal();

	KcRenderObject* obj = hasNormal ? new KcLightenObject(geom->type()) : new KcRenderObject(geom->type());

	if (hasNormal) {
		//((KcLightenObject*)obj)->setNormalMatrix(camera_.getNormalMatrix());
		((KcLightenObject*)obj)->setNormalMatrix(mat4::identity());
	}

	auto vbo = std::make_shared<KcGpuBuffer>();
	vbo->setData(geom->vertexData(), geom->vertexCount() * geom->vertexSize(), KcGpuBuffer::k_stream_draw);

	obj->setVBO(vbo, decl);
	if (geom->indexCount() > 0) {
		auto ibo = std::make_shared<KcGpuBuffer>(KcGpuBuffer::k_index_buffer);
		ibo->setData(geom->indexData(), geom->indexCount() * geom->indexSize(), KcGpuBuffer::k_stream_draw);
		obj->setIBO(ibo, geom->indexCount());
	}

	obj->setColor(clr_);
	obj->setProjMatrix(camera_.getMvpMat());
	if (curClipBox_ != -1) {
		auto& box = clipBoxHistList_[curClipBox_];
		obj->setClipBox({ box.lower(), box.upper() });
	}

	currentRenderList().objs.emplace_back(obj);
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

	super_::pushClipRect(cr); // TODO: 该调用后续可以去掉，不改动ImGui的渲染状态
}


void KcImOglPaint::popClipRect()
{
	clipRectStack_.pop_back();

	super_::popClipRect();
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


void KcImOglPaint::drawRenderList_()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	unsigned viewport(-1), clipRect(-1), clipBox(-2);
	for (auto& rd : renderList_) {
		auto& state = rd.first;
		if (std::get<0>(state) != viewport) {
			viewport = std::get<0>(state);
			glViewport_(viewport);
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

		KcGlslProgram::useProgram(0); // 禁用shader，使用固定管线绘制

		for (auto& i : rl.fns)
			i();

		pushTextVbo_(rl);
		for (auto& i : rl.objs)
			i->draw();
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
