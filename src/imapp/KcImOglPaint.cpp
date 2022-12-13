#include "KcImOglPaint.h"
#include "imgui.h"
#include "glad.h"
#include "opengl/KcGlslProgram.h"
#include "opengl/KcGlslShader.h"
#include "opengl/KcGpuBuffer.h"
#include "opengl/KcVertexDeclaration.h"


namespace kPrivate
{
	const static char* vertex_shader_glsl_130 =
		"uniform mat4 matMvp;\n"
		"in vec2 Position;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = matMvp * vec4(Position.xyz, 1);\n"
		"}\n";
}


void oglDrawCallback(const ImDrawList* parent_list, const ImDrawCmd* cmd)
{
	auto objs = (std::vector<std::unique_ptr<KcRenderObject>>*)cmd->UserCallbackData;
	for (auto& i : *objs)
		i->draw();
}


void KcImOglPaint::drawLineStrip(point_getter fn, unsigned count)
{
	auto prog = std::make_shared<KcGlslProgram>();
	prog->attachShader(new KcGlslShader(KcGlslShader::k_shader_vertex, kPrivate::vertex_shader_glsl_130));
	prog->shaderAt(0)->compile();
	prog->link();

	objs_.emplace_back(new KcRenderObject(KcRenderObject::k_line_strip, prog));

	auto decl = std::make_shared<KcVertexDeclaration>();
	KcVertexAttribute attr(0, KcVertexAttribute::k_float3, 0, KcVertexAttribute::k_position);
	decl->pushAttribute(attr);

	auto vbo = std::make_shared<KcGpuBuffer>();
	std::vector<point3f> vtx;
	for (unsigned i = 0; i < count; i++) // ◊∞≈‰ ˝æ›
		vtx.push_back(fn(i));
	vbo->setData(vtx.data(), vtx.size() * sizeof(point3f), KcGpuBuffer::k_stream_draw);
	objs_.back()->setVbo(vbo, decl);
	objs_.back()->setProjMatrix(camera_.getMvpMat());
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
		dl->AddCallback(oglDrawCallback, &objs_);
		dl->AddCallback(ImDrawCallback_ResetRenderState, nullptr); // »√imguiª÷∏¥‰÷»æ◊¥Ã¨
	}

	super_::endPaint();
}
