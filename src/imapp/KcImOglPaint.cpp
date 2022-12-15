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
	void oglDrawLine(int style, const KcImOglPaint::point3& from, const KcImOglPaint::point3& to)
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
	vp.lower().y() = ImGui::GetWindowViewport()->Size.y - (vp.upper().y() + vp.height());
	vp.setExtent(1, viewport().height());
	obj->setViewport(vp);

	objs_.emplace_back(obj);
}


void KcImOglPaint::setGlViewport_(const rect_t& rc)
{
	auto draw_data = ImGui::GetDrawData();
	int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
	auto vp(rc);
	vp.lower().y() = fb_height - (vp.upper().y() + vp.height());
	vp.setExtent(1, viewport().height());
	glViewport(vp.lower().x(), vp.lower().y(), vp.width(), vp.height());

	// TODO: glViewportArrayv();
}


void KcImOglPaint::drawPoint(const point3& pt)
{
	auto clr = clr_;
	auto ptSize = pointSize_;
	auto projMat = camera_.projMatrix();
	auto viewModelMat = camera_.getMvMat();
	auto vp = viewport();
	auto drawFn = [clr, ptSize, pt, projMat, viewModelMat, vp, this]() {
		auto progId = KcGlslProgram::currentProgram();

		KcGlslProgram::useProgram(0); // 禁用shader，使用固定管线绘制

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixd(projMat.data());
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixd(viewModelMat.data());
		setGlViewport_(vp);

		glColor4f(clr.r(), clr.g(), clr.b(), clr.a());
		glPointSize(ptSize);
		glEnable(GL_POINT_SMOOTH);
		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBegin(GL_POINTS);
		glVertex3f(pt.x(), pt.y(), pt.z());
		glEnd();

		KcGlslProgram::useProgram(progId);
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
	auto projMat = camera_.projMatrix();
	auto viewModelMat = camera_.getMvMat();
	auto vp = viewport();
	auto drawFn = [clr, lnWidth, from, to, style, projMat, viewModelMat, vp, this]() {
		auto progId = KcGlslProgram::currentProgram();

		KcGlslProgram::useProgram(0); // 禁用shader，使用固定管线绘制

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixd(projMat.data());
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixd(viewModelMat.data());
		setGlViewport_(vp);

		glLineWidth(lnWidth);
		glColor4f(clr.r(), clr.g(), clr.b(), clr.a());
		//glEnable(GL_LINE_SMOOTH);
		//glHint(GL_LINE_SMOOTH, GL_NICEST);
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBegin(GL_LINES);
		glVertex3f(from.x(), from.y(), from.z());
		glVertex3f(to.x(), to.y(), to.z());
		glEnd();

		KcGlslProgram::useProgram(progId);
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