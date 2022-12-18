#include "KcImOglPaint.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "glad.h"
#include "opengl/KcGlslProgram.h"
#include "opengl/KcGlslShader.h"
#include "opengl/KcGpuBuffer.h"
#include "opengl/KcVertexDeclaration.h"
#include "opengl/KcPointObject.h"
#include "opengl/KcLineObject.h"
#include "opengl/KsShaderManager.h"


namespace kPrivate
{
	void oglSetRenderState(const ImDrawList* parent_list, const ImDrawCmd* cmd)
	{
		KcGlslProgram::useProgram(0); // 禁用shader，使用固定管线绘制

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		KcImOglPaint* paint = (KcImOglPaint*)cmd->UserCallbackData;
		auto rc = paint->viewport();
		auto y0 = ImGui::GetMainViewport()->Size.y - rc.upper().y();
		glViewport(rc.lower().x(), y0, rc.width(), rc.height());
	}

	void oglDrawVbos(const ImDrawList* parent_list, const ImDrawCmd* cmd)
	{
		auto objs = (std::vector<std::unique_ptr<KcRenderObject>>*)cmd->UserCallbackData;
		for (auto& i : *objs)
			i->draw();
	}

	void oglDrawFns(const ImDrawList* parent_list, const ImDrawCmd* cmd)
	{
		auto fns = (std::vector<std::function<void(void)>>*)cmd->UserCallbackData;
		for (auto& i : *fns)
			i();
	}

	void oglDrawText(const ImDrawList* parent_list, const ImDrawCmd* cmd)
	{
		auto vtx = (std::vector<KcImOglPaint::TextVbo>*)cmd->UserCallbackData;

		auto obj = KcRenderObject(KcRenderObject::k_quads, KsShaderManager::singleton().programColorUV());

		auto decl = std::make_shared<KcVertexDeclaration>();
		KcVertexAttribute attrPos(0, KcVertexAttribute::k_float3, 0, KcVertexAttribute::k_position);
		KcVertexAttribute attrUv(1, KcVertexAttribute::k_float2, sizeof(float) * 3, KcVertexAttribute::k_texcoord);
		KcVertexAttribute attrClr(2, KcVertexAttribute::k_float4, sizeof(float) * 5, KcVertexAttribute::k_diffuse);
		decl->pushAttribute(attrPos);
		decl->pushAttribute(attrUv);
		decl->pushAttribute(attrClr);

		auto vbo = std::make_shared<KcGpuBuffer>();
		vbo->setData(vtx->data(), vtx->size() * sizeof(KcImOglPaint::TextVbo), KcGpuBuffer::k_stream_draw);

		obj.setVbo(vbo, decl);
		obj.setProjMatrix(float4x4<>::identity());
		obj.draw();
	}

	// color和width已设置好，此处主要设置style
	void oglLine(int style, const KcImOglPaint::point3& from, const KcImOglPaint::point3& to)
	{
		glBegin(GL_LINES);
		glVertex3f(from.x(), from.y(), from.z());
		glVertex3f(to.x(), to.y(), to.z());
		glEnd();
	}
}

void KcImOglPaint::beginPaint()
{
	fns_.clear();
	objs_.clear();
	super_::beginPaint();
}


void KcImOglPaint::endPaint()
{
	if (!fns_.empty() || !objs_.empty()) {
		auto dl = ImGui::GetWindowDrawList();

		dl->AddCallback(kPrivate::oglSetRenderState, this);

		// 绘制零散点线
		if (!fns_.empty())
			dl->AddCallback(kPrivate::oglDrawFns, &fns_);

		// 绘制text
		if (!texts_.empty())
			dl->AddCallback(kPrivate::oglDrawText, &texts_);

		// 绘制plottables
		if (!objs_.empty())
		    dl->AddCallback(kPrivate::oglDrawVbos, &objs_);

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

	auto vp = viewport();
	auto drawFn = [clr, ptSize, p, vp, this]() {

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

	fns_.push_back(drawFn);
}


void KcImOglPaint::drawPoints(point_getter fn, unsigned count)
{
	auto obj = new KcPointObject;

	auto decl = std::make_shared<KcVertexDeclaration>();
	KcVertexAttribute attr(0, KcVertexAttribute::k_float3, 0, KcVertexAttribute::k_position);
	decl->pushAttribute(attr);

	auto vbo = std::make_shared<KcGpuBuffer>();
	std::vector<point3f> vtx;
	for (unsigned i = 0; i < count; i++) // 装配数据
		vtx.push_back(fn(i));
	vbo->setData(vtx.data(), vtx.size() * sizeof(point3f), KcGpuBuffer::k_stream_draw);

	obj->setVbo(vbo, decl);
	obj->setColor(clr_);
	obj->setSize(pointSize_);
	obj->setProjMatrix(camera_.getMvpMat());
	objs_.emplace_back(obj);
}


void KcImOglPaint::drawLine(const point3& from, const point3& to)
{
	auto clr = clr_;
	auto lnWidth = lineWidth_;
	auto style = lineStyle_;
	auto vp = viewport();
	auto pt0 = toNdc_(from);
	auto pt1 = toNdc_(to);

	auto drawFn = [clr, lnWidth, pt0, pt1, style, vp, this]() {

		glLineWidth(lnWidth);
		glColor4f(clr.r(), clr.g(), clr.b(), clr.a());
		//glEnable(GL_LINE_SMOOTH);
		//glHint(GL_LINE_SMOOTH, GL_NICEST);
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		kPrivate::oglLine(style, pt0, pt1);

	};

	fns_.push_back(drawFn);
}


void KcImOglPaint::drawLineStrip(point_getter fn, unsigned count)
{
	auto obj = new KcLineObject(KcRenderObject::k_line_strip);

	auto decl = std::make_shared<KcVertexDeclaration>();
	KcVertexAttribute attr(0, KcVertexAttribute::k_float3, 0, KcVertexAttribute::k_position);
	decl->pushAttribute(attr);

	auto vbo = std::make_shared<KcGpuBuffer>();
	std::vector<point3f> vtx;
	for (unsigned i = 0; i < count; i++) // 装配数据
		vtx.push_back(fn(i));
	vbo->setData(vtx.data(), vtx.size() * sizeof(point3f), KcGpuBuffer::k_stream_draw);

	obj->setVbo(vbo, decl);
	obj->setColor(clr_);
	obj->setWidth(lineWidth_);
	obj->setProjMatrix(camera_.getMvpMat());
	objs_.emplace_back(obj);
}


void KcImOglPaint::drawText(const point3& topLeft, const point3& hDir, const point3& vDir, const char* text)
{
	auto font = ImGui::GetFont();
	auto eos = text + strlen(text);

	auto hScale = 1.0 / unprojectv(hDir).length();
	auto vScale = 1.0 / unprojectv(vDir).length();
	auto height = vDir * font->FontSize * vScale; // 每行文字的高度. 暂时只支持单行渲染，该变量用不上

	auto s = text;
	auto orig = topLeft;
	texts_.reserve(texts_.size() + (eos - text) * 4);
	while (s < eos) {
		// Decode and advance source
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

			texts_.resize(texts_.size() + 4); // 按quad原语绘制
			TextVbo* buf = texts_.data() + texts_.size() - 4;

			// 文字框的4个顶点对齐glyph
			buf[0].pos = toNdc_(orig + hScale * glyph->X0 + vScale * glyph->Y0); // top-left
			buf[1].pos = toNdc_(orig + hScale * glyph->X1 + vScale * glyph->Y0); // top-right
			buf[2].pos = toNdc_(orig + hScale * glyph->X1 + vScale * glyph->Y1); // bottom-right
			buf[3].pos = toNdc_(orig + hScale * glyph->X0 + vScale * glyph->Y1); // bottom-left

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

		orig += hDir * glyph->AdvanceX;
	}
}
