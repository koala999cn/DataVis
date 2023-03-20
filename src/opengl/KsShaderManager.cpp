#include "KsShaderManager.h"
#include "KcGlslShader.h"
#include "KcGlslProgram.h"
#include <assert.h>


KsShaderManager::program_ptr KsShaderManager::flatVersion(const program_ptr& prog, bool flat)
{
	if (prog == progColor_[0] || prog == progColor_[1])
		return progColor_[flat];

	if (prog == progColorUV_[0] || prog == progColorUV_[1])
		return progColorUV_[flat];

	if (prog == progMonoLight_[0] || prog == progMonoLight_[1])
		return progMonoLight_[flat];

	return prog; // TODO: prog可能为nullptr
}


KsShaderManager::shader_ptr KsShaderManager::vsMono()
{
	const static char* vertex_shader_mono =
		"uniform mat4 matMvp;\n"
		"uniform vec4 vColor;\n"
		"uniform int  iEnableClip;\n"
		"uniform vec3 vClipLower;\n"
		"uniform vec3 vClipUpper;\n"	
		"in vec3 iPosition;\n"
		"flat out vec4 Frag_Color;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = matMvp * vec4(iPosition, 1);\n"
		"    Frag_Color = vColor;\n"
		"    if (iEnableClip != 0)\n"
	    "    {\n"
		"        gl_ClipDistance[0] = iPosition.x - vClipLower.x;\n"
		"        gl_ClipDistance[1] = iPosition.y - vClipLower.y;\n"
		"        gl_ClipDistance[2] = iPosition.z - vClipLower.z;\n"
		"        gl_ClipDistance[3] = vClipUpper.x - iPosition.x;\n"
		"        gl_ClipDistance[4] = vClipUpper.y - iPosition.y;\n"
		"        gl_ClipDistance[5] = vClipUpper.z - iPosition.z;\n"
		"    }\n"
		"}\n";

	if (vsMono_ == nullptr) {
		vsMono_ = std::make_shared<KcGlslShader>(KcGlslShader::k_shader_vertex, vertex_shader_mono);
		auto info = vsMono_->infoLog();
		assert(vsMono_->compileStatus());
	}

	return vsMono_;
}


KsShaderManager::shader_ptr KsShaderManager::vsColor(bool flat)
{
	const static char* vertex_shader_color =
		"out vec4 Frag_Color;\n"
		"uniform mat4 matMvp;\n"
		"uniform int  iEnableClip;\n"
		"uniform vec3 vClipLower;\n"
		"uniform vec3 vClipUpper;\n"
		"in vec3 iPosition;\n"
		"in vec4 iColor;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = matMvp * vec4(iPosition, 1);\n"
		"    Frag_Color = iColor;\n"
		"    if (iEnableClip != 0)\n"
		"    {\n"
		"        gl_ClipDistance[0] = iPosition.x - vClipLower.x;\n"
		"        gl_ClipDistance[1] = iPosition.y - vClipLower.y;\n"
		"        gl_ClipDistance[2] = iPosition.z - vClipLower.z;\n"
		"        gl_ClipDistance[3] = vClipUpper.x - iPosition.x;\n"
		"        gl_ClipDistance[4] = vClipUpper.y - iPosition.y;\n"
		"        gl_ClipDistance[5] = vClipUpper.z - iPosition.z;\n"
		"    }\n"
		"}\n";

	if (vsColor_[flat] == nullptr) {
		vsColor_[flat] = createShader_(KcGlslShader::k_shader_vertex, vertex_shader_color, flat);
		auto info = vsColor_[flat]->infoLog();
		assert(vsColor_[flat]->compileStatus());
	}

	return vsColor_[flat];
}


KsShaderManager::shader_ptr KsShaderManager::vsColorUV(bool flat)
{
	const static char* vertex_shader_color_uv =
		"out vec4 Frag_Color;\n"
		"out vec2 Frag_UV;\n"
		"uniform mat4 matMvp;\n"
		"layout (location = 0) in vec3 iPosition;\n"
		"layout (location = 1) in vec2 iUV;\n"
		"layout (location = 2) in vec4 iColor;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = matMvp * vec4(iPosition, 1);\n"
		"    Frag_UV = iUV;\n"
		"    Frag_Color = iColor;\n"
		"}\n";

	if (vsColorUV_[flat] == nullptr) {
		vsColorUV_[flat] = createShader_(KcGlslShader::k_shader_vertex, vertex_shader_color_uv, flat);
		auto info = vsColorUV_[flat]->infoLog();
		assert(vsColorUV_[flat]->compileStatus());
	}

	return vsColorUV_[flat];
}


KsShaderManager::shader_ptr KsShaderManager::vsInst2d()
{
	const static char* vertex_shader_inst2d =
		"uniform mat4 matMvp;\n"
		"uniform vec4 vColor;\n"
		"uniform vec2 vScale;\n"
		"layout (location = 0) in vec2 iPosition;\n"
		"layout (location = 1) in vec3 iOffset;\n"
		"flat out vec4 Frag_Color;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = matMvp * vec4(iOffset + vec3(iPosition * vScale, 0), 1);\n"
		"    Frag_Color = vColor;\n"
		"}\n";

	if (vsInst2d_ == nullptr) {
		vsInst2d_ = std::make_shared<KcGlslShader>(KcGlslShader::k_shader_vertex, vertex_shader_inst2d);
		auto info = vsInst2d_->infoLog();
		assert(vsInst2d_->compileStatus());
	}

	return vsInst2d_;
}


KsShaderManager::shader_ptr KsShaderManager::vsInst2dColor()
{
	const static char* vertex_shader_inst2d_color =
		"uniform mat4 matMvp;\n"
		"uniform vec2 vScale;\n"
		"layout (location = 0) in vec2 iPosition;\n"
		"layout (location = 1) in vec4 iColor;\n"
		"layout (location = 2) in vec3 iOffset;\n"
		"flat out vec4 Frag_Color;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = matMvp * vec4(iOffset + vec3(iPosition * vScale, 0), 1);\n"
		"    Frag_Color = iColor;\n"
		"}\n";

	if (vsInst2dColor_ == nullptr) {
		vsInst2dColor_ = std::make_shared<KcGlslShader>(KcGlslShader::k_shader_vertex, vertex_shader_inst2d_color);
		auto info = vsInst2dColor_->infoLog();
		assert(vsInst2dColor_->compileStatus());
	}

	return vsInst2dColor_;
}


KsShaderManager::shader_ptr KsShaderManager::vsMonoLight(bool flat)
{
	const static char* vertex_shader_mono_light =
		"out vec4 Frag_Color;\n"
		"uniform mat4 matMvp;\n"
		"uniform mat4 matNormal;\n"
		"uniform vec4 vColor;\n"
		"uniform int  iEnableClip;\n"
		"uniform vec3 vClipLower;\n"
		"uniform vec3 vClipUpper;\n"
		"in vec3 iPosition;\n"
		"in vec3 iNormal;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = matMvp * vec4(iPosition, 1);\n"
		"    vec3 vNorm = normalize(matNormal * vec4(iNormal, 0)).xyz;\n"
		"    vec3 vLightDir = vec3(1.0, 1.0, 1.0);\n"
		"    float fDot = max(0.0, dot(vNorm, vLightDir));\n"
		"    Frag_Color.rgb = vColor.rgb * fDot;\n"
		"    Frag_Color.a = vColor.a;\n"
		"    if (iEnableClip != 0)\n"
		"    {\n"
		"        gl_ClipDistance[0] = iPosition.x - vClipLower.x;\n"
		"        gl_ClipDistance[1] = iPosition.y - vClipLower.y;\n"
		"        gl_ClipDistance[2] = iPosition.z - vClipLower.z;\n"
		"        gl_ClipDistance[3] = vClipUpper.x - iPosition.x;\n"
		"        gl_ClipDistance[4] = vClipUpper.y - iPosition.y;\n"
		"        gl_ClipDistance[5] = vClipUpper.z - iPosition.z;\n"
		"    }\n"
		"}\n";

	if (vsMonoLight_[flat] == nullptr) {
		vsMonoLight_[flat] = createShader_(KcGlslShader::k_shader_vertex, vertex_shader_mono_light, flat);
		auto info = vsMonoLight_[flat]->infoLog();
		assert(vsMonoLight_[flat]->compileStatus());
	}

	return vsMonoLight_[flat];
}


KsShaderManager::shader_ptr KsShaderManager::fsNavie(bool flat)
{
	const static char* frag_shader_navie =
		"in vec4 Frag_Color;\n"
		"void main()\n"
		"{\n"
		"    gl_FragColor = Frag_Color;\n"
		"}\n";

	if (fsNavie_[flat] == nullptr) {
		fsNavie_[flat] = createShader_(KcGlslShader::k_shader_fragment, frag_shader_navie, flat);
		auto info = fsNavie_[flat]->infoLog();
		assert(fsNavie_[flat]->compileStatus());
	}

	return fsNavie_[flat];
}


KsShaderManager::shader_ptr KsShaderManager::fsColorUV(bool flat)
{
	const static char* frag_shader_color_uv =
		"in vec4 Frag_Color;\n"
		"in vec2 Frag_UV;\n"
	    "uniform sampler2D Texture;\n"	
		"void main()\n"
		"{\n"
		"    gl_FragColor = Frag_Color * texture2D(Texture, Frag_UV.st);\n"
		"}\n";

	if (fsColorUV_[flat] == nullptr) {
		fsColorUV_[flat] = createShader_(KcGlslShader::k_shader_fragment, frag_shader_color_uv, flat);
		assert(fsColorUV_[flat]->compileStatus());
	}

	return fsColorUV_[flat];
}


KsShaderManager::program_ptr KsShaderManager::progMono()
{
	return createProg_(progMono_, vsMono(), fsNavie(true));
}


KsShaderManager::program_ptr KsShaderManager::progColor(bool flat)
{
	return createProg_(progColor_[flat], vsColor(flat), fsNavie(flat));
}


KsShaderManager::program_ptr KsShaderManager::progColorUV(bool flat)
{
	return createProg_(progColorUV_[flat], vsColorUV(flat), fsColorUV(flat));
}


KsShaderManager::program_ptr KsShaderManager::progInst2d()
{
	return createProg_(progInst2d_, vsInst2d(), fsNavie(true));
}


KsShaderManager::program_ptr KsShaderManager::progInst2dColor()
{
	return createProg_(progInst2dColor_, vsInst2dColor(), fsNavie(true));
}


KsShaderManager::program_ptr KsShaderManager::progMonoLight(bool flat)
{
	return createProg_(progMonoLight_[flat], vsMonoLight(flat), fsNavie(flat));
}


KsShaderManager::shader_ptr KsShaderManager::createShader_(int type, const char* source, bool flat)
{
	shader_ptr shader;

	if (flat) { // flat模式下，增加flat关键字
		std::string src("flat ");
		src += source;
		shader = std::make_shared<KcGlslShader>(KcGlslShader::KeType(type), src);
	}
	else {
		shader = std::make_shared<KcGlslShader>(KcGlslShader::KeType(type), source);
	}

	return shader;
}


KsShaderManager::program_ptr KsShaderManager::createProg_(program_ptr& out, const shader_ptr& vs, const shader_ptr& fs)
{
	if (out == nullptr) {
		out = std::make_shared<KcGlslProgram>();
		out->attachShader(vs);
		out->attachShader(fs);
		out->link();
		auto info = out->infoLog();
		assert(out->linked() && out->linkStatus());
	}

	return out;
}
