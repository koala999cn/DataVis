#include "KcImOglPaint.h"
#include "imgui.h"
#include "glad.h"
#include "opengl/KcGlslProgram.h"
#include "opengl/KcGlslShader.h"
#include "opengl/KcBufferObject.h"


namespace kPrivate
{
	const static char* vertex_shader_glsl_130 =
		"uniform mat4 ProjMtx;\n"
		"in vec2 Position;\n"
		"in vec4 Color;\n"
		"out vec4 Frag_Color;\n"
		"void main()\n"
		"{\n"
		"    Frag_Color = Color;\n"
		"    gl_Position = ProjMtx * vec4(Position.xy, 0, 1);\n"
		"}\n";

	const static char* fragment_shader_glsl_130 =
		"in vec4 Frag_Color;\n"
		"out vec4 Out_Color;\n"
		"void main()\n"
		"{\n"
		"    Out_Color = Frag_Color;\n"
		"}\n";
}


void oglDrawLineStrip(const ImDrawList* parent_list, const ImDrawCmd* cmd)
{
	KcGlslProgram prog;
	prog.attachShader(new KcGlslShader(KcGlslShader::k_shader_vertex, kPrivate::vertex_shader_glsl_130));
	prog.attachShader(new KcGlslShader(KcGlslShader::k_shader_fragment, kPrivate::fragment_shader_glsl_130));
	std::string info;

	if (!prog.shaderAt(0)->compile())
		info = prog.shaderAt(0)->infoLog();
	prog.shaderAt(1)->compile();
	prog.link();

	auto oldProg = prog.currentProgram();
	prog.useProgram();

	cmd->UserCallbackData;
	
	int count(100);
	glDrawArrays(GL_LINE_STRIP, 0, count);

	prog.useProgram(oldProg);
}

void KcImOglPaint::drawLineStrip(point_getter fn, unsigned count)
{
	auto dl = ImGui::GetWindowDrawList();
	dl->AddCallback(oglDrawLineStrip, nullptr);
}
