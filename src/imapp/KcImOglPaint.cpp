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
	void oglDrawCallback(const ImDrawList* parent_list, const ImDrawCmd* cmd)
	{
		auto objs = (std::vector<std::unique_ptr<KcRenderObject>>*)cmd->UserCallbackData;
		for (auto& i : *objs)
			i->draw();
	}
}

void KcImOglPaint::beginPaint()
{
	objs_.clear();
	super_::beginPaint();
}


void KcImOglPaint::endPaint()
{
	if (!objs_.empty()) {
		auto dl = ImGui::GetWindowDrawList();
		dl->AddCallback(kPrivate::oglDrawCallback, &objs_);
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


void KcImOglPaint::drawPoint(const point3& pt)
{
	super_::drawPoint(pt);

#if 0 // TODO:
	auto progId = KcGlslProgram::currentProgram();

	KcGlslProgram::useProgram(0); // 禁用shader，使用固定管线绘制

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(camera_.projMatrix().data());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(camera_.viewMatrix().data());

	glPointSize(pointSize_);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBegin(GL_POINTS);
	glColor4f(clr_.r(), clr_.g(), clr_.b(), clr_.a());
	glVertex3f(pt.x(), pt.y(), pt.z());
	glEnd();

	KcGlslProgram::useProgram(progId);
#endif
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