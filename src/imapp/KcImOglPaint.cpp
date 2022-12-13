#include "KcImOglPaint.h"
#include "imgui.h"
#include "glad.h"
#include "opengl/KcGlslProgram.h"
#include "opengl/KcGlslShader.h"
#include "opengl/KcGpuBuffer.h"
#include "opengl/KcVertexDeclaration.h"
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
	vp.lower().y() = ImGui::GetWindowViewport()->Size.y - vp.upper().y() - vp.height(); // TODO: ???
	vp.setExtent(1, viewport().height());
	obj->setViewport(vp);

	objs_.emplace_back(obj);
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
	pushRenderObject_(obj);
}