#include "KcImOglPaint.h"
#include "imgui.h"
#include "glad.h"
#include "opengl/KcGlslProgram.h"
#include "opengl/KcGlslShader.h"
#include "opengl/KcGpuBuffer.h"
#include "opengl/KcVertexDeclaration.h"
#include "opengl/KcPointObject.h"
#include "opengl/KcLineObject.h"


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

		if (!fns_.empty())
			dl->AddCallback(kPrivate::oglDrawFns, &fns_);

		if (!objs_.empty())
		    dl->AddCallback(kPrivate::oglDrawVbos, &objs_);

		dl->AddCallback(ImDrawCallback_ResetRenderState, nullptr); // 让imgui恢复渲染状态
	}

	super_::endPaint();
}


void KcImOglPaint::pushRenderObject_(KcRenderObject* obj)
{
	obj->setProjMatrix(camera_.getMvpMat());

	auto vp = viewport(); // opengl的viewport原点在左下角，此处要反转y值
	//vp.lower().y() = ImGui::GetWindowViewport()->Size.y - (vp.lower().y() + vp.height());
	//vp.setExtent(1, viewport().height());
	//obj->setViewport(vp);

	objs_.emplace_back(obj);
}


void KcImOglPaint::setGlViewport_(const rect_t& rc)
{
	auto draw_data = ImGui::GetDrawData();
	int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
	auto vp(rc);
	vp.lower().y() = fb_height - (vp.lower().y() + vp.height());
	vp.setExtent(1, viewport().height());
	glViewport(vp.lower().x(), vp.lower().y(), vp.width(), vp.height());

	// TODO: glViewportArrayv();
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
	pushRenderObject_(obj);
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
	pushRenderObject_(obj);
}