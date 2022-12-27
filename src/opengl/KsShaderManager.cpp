#include "KsShaderManager.h"
#include "KcGlslShader.h"
#include "KcGlslProgram.h"
#include <assert.h>


KsShaderManager::KsShaderManager()
{

}


KsShaderManager::~KsShaderManager()
{

}


KsShaderManager::shader_ptr KsShaderManager::vertexShaderFlat()
{
	const static char* vertex_shader_flat =
		"uniform mat4 matMvp;\n"
		"uniform vec4 vColor;\n"
		"uniform int  iEnableClip;\n"
		"uniform vec3 vClipLower;\n"
		"uniform vec3 vClipUpper;\n"	
		"in vec3 iPosition;\n"
		"out vec4 Frag_Color;\n"
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

	if (vertexShaderFlat_ == nullptr) {
		vertexShaderFlat_ = std::make_shared<KcGlslShader>(KcGlslShader::k_shader_vertex, vertex_shader_flat);
		auto info = vertexShaderFlat_->infoLog();
		assert(vertexShaderFlat_->compileStatus());
	}

	return vertexShaderFlat_;
}


KsShaderManager::shader_ptr KsShaderManager::vertexShaderSmooth()
{
	const static char* vertex_shader_smooth =
		"uniform mat4 matMvp;\n"
		"uniform int  iEnableClip;\n"
		"uniform vec3 vClipLower;\n"
		"uniform vec3 vClipUpper;\n"
		"in vec3 iPosition;\n"
		"in vec4 iColor;\n"
		"out vec4 Frag_Color;\n"
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

	if (vertexShaderSmooth_ == nullptr) {
		vertexShaderSmooth_ = std::make_shared<KcGlslShader>(KcGlslShader::k_shader_vertex, vertex_shader_smooth);
		auto info = vertexShaderSmooth_->infoLog();
		assert(vertexShaderSmooth_->compileStatus());
	}

	return vertexShaderSmooth_;
}


KsShaderManager::shader_ptr KsShaderManager::vertexShaderSmoothUV()
{
	const static char* vertex_shader_smooth_uv =
		"uniform mat4 matMvp;\n"
		"layout (location = 0) in vec3 iPosition;\n"
		"layout (location = 1) in vec2 iUV;\n"
		"layout (location = 2) in vec4 iColor;\n"
		"out vec2 Frag_UV;\n"
		"out vec4 Frag_Color;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = matMvp * vec4(iPosition, 1);\n"
		"    Frag_UV = iUV;\n"
		"    Frag_Color = iColor;\n"
		"}\n";

	if (vertexShaderSmoothUV_ == nullptr) {
		vertexShaderSmoothUV_ = std::make_shared<KcGlslShader>(KcGlslShader::k_shader_vertex, vertex_shader_smooth_uv);
		auto info = vertexShaderSmoothUV_->infoLog();
		assert(vertexShaderSmoothUV_->compileStatus());
	}

	return vertexShaderSmoothUV_;
}


KsShaderManager::shader_ptr KsShaderManager::vertexShaderFlatLight()
{
	const static char* vertex_shader_flat_light =
		"uniform mat4 matMvp;\n"
		"uniform mat4 matNormal;\n"
		"uniform vec4 vColor;\n"
		"uniform int  iEnableClip;\n"
		"uniform vec3 vClipLower;\n"
		"uniform vec3 vClipUpper;\n"
		"in vec3 iPosition;\n"
		"in vec3 iNormal;\n"
		"out vec4 Frag_Color;\n"
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

	if (vertexShaderFlatLight_ == nullptr) {
		vertexShaderFlatLight_ = std::make_shared<KcGlslShader>(KcGlslShader::k_shader_vertex, vertex_shader_flat_light);
		auto info = vertexShaderFlatLight_->infoLog();
		assert(vertexShaderFlatLight_->compileStatus());
	}

	return vertexShaderFlatLight_;
}


KsShaderManager::shader_ptr KsShaderManager::fragShaderFlat()
{
	const static char* frag_shader_flat =
		"varying vec4 Frag_Color;\n"
		"void main()\n"
		"{\n"
		"    gl_FragColor = Frag_Color;\n"
		"}\n";

	if (fragShaderFlat_ == nullptr) {
		fragShaderFlat_ = std::make_shared<KcGlslShader>(KcGlslShader::k_shader_fragment, frag_shader_flat);
		assert(fragShaderFlat_->compileStatus());
	}

	return fragShaderFlat_;
}


KsShaderManager::shader_ptr KsShaderManager::fragShaderSmoothUV()
{
	const static char* frag_shader_smooth_uv =
	    "uniform sampler2D Texture;\n"
		"varying vec2 Frag_UV;\n"
		"varying vec4 Frag_Color;\n"
		"void main()\n"
		"{\n"
		"    gl_FragColor = Frag_Color * texture2D(Texture, Frag_UV.st);\n"
		"}\n";

	if (fragShaderSmoothUV_ == nullptr) {
		fragShaderSmoothUV_ = std::make_shared<KcGlslShader>(KcGlslShader::k_shader_fragment, frag_shader_smooth_uv);
		assert(fragShaderSmoothUV_->compileStatus());
	}

	return fragShaderSmoothUV_;
}


KsShaderManager::program_ptr KsShaderManager::programFlat()
{
	if (progFlat_ == nullptr) {
		progFlat_ = std::make_shared<KcGlslProgram>();
		progFlat_->attachShader(vertexShaderFlat());
		progFlat_->attachShader(fragShaderFlat());
		progFlat_->link();
		assert(progFlat_->linked() && progFlat_->linkStatus());
	}

	return progFlat_;
}


KsShaderManager::program_ptr KsShaderManager::programSmooth()
{
	if (progSmooth_ == nullptr) {
		progSmooth_ = std::make_shared<KcGlslProgram>();
		progSmooth_->attachShader(vertexShaderSmooth());
		progSmooth_->attachShader(fragShaderFlat());
		progSmooth_->link();
		assert(progSmooth_->linked() && progSmooth_->linkStatus());
	}

	return progSmooth_;
}


KsShaderManager::program_ptr KsShaderManager::programSmoothUV()
{
	if (progSmoothUV_ == nullptr) {
		progSmoothUV_ = std::make_shared<KcGlslProgram>();
		progSmoothUV_->attachShader(vertexShaderSmoothUV());
		progSmoothUV_->attachShader(fragShaderSmoothUV());
		progSmoothUV_->link();
		assert(progSmoothUV_->linked() && progSmoothUV_->linkStatus());
	}

	return progSmoothUV_;
}


KsShaderManager::program_ptr KsShaderManager::programFlatLight()
{
	if (progFlatLight_ == nullptr) {
		progFlatLight_ = std::make_shared<KcGlslProgram>();
		progFlatLight_->attachShader(vertexShaderFlatLight());
		progFlatLight_->attachShader(fragShaderFlat());
		progFlatLight_->link();
		auto info = progFlatLight_->infoLog();
		assert(progFlatLight_->linked() && progFlatLight_->linkStatus());
	}

	return progFlatLight_;
}