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


KsShaderManager::shader_ptr KsShaderManager::vertexShaderMono()
{
	const static char* vertex_shader_mono =
		"uniform mat4 mvpMat;\n"
		"uniform int  iEnableClip;\n"
		"uniform vec3 vClipLower;\n"
		"uniform vec3 vClipUpper;\n"
		"uniform vec4 vColor;\n"
		"in vec3 iPosition;\n"
		"out vec4 Frag_Color;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = mvpMat * vec4(iPosition, 1);\n"
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

	if (vertexShaderMono_ == nullptr) {
		vertexShaderMono_ = std::make_shared<KcGlslShader>(KcGlslShader::k_shader_vertex, vertex_shader_mono);
		if (!vertexShaderMono_->compileStatus()) {
			auto info = vertexShaderMono_->infoLog();
			assert(false && info.c_str());
		}
	}

	return vertexShaderMono_;
}


KsShaderManager::shader_ptr KsShaderManager::vertexShaderColorUV()
{
	const static char* vertex_shader_color_uv =
		"uniform mat4 mvpMat;\n"
		"layout (location = 0) in vec3 iPosition;\n"
		"layout (location = 1) in vec2 iUV;\n"
		"layout (location = 2) in vec4 iColor;\n"
		"out vec2 Frag_UV;\n"
		"out vec4 Frag_Color;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = mvpMat * vec4(iPosition, 1);\n"
		"    Frag_UV = iUV;\n"
		"    Frag_Color = iColor;\n"
		"}\n";

	if (vertexShaderColorUV_ == nullptr) {
		vertexShaderColorUV_ = std::make_shared<KcGlslShader>(KcGlslShader::k_shader_vertex, vertex_shader_color_uv);
		assert(vertexShaderColorUV_->compileStatus());
	}

	return vertexShaderColorUV_;
}


KsShaderManager::shader_ptr KsShaderManager::fragShaderNaive()
{
	const static char* frag_shader_naive =
		"varying vec4 Frag_Color;\n"
		"void main()\n"
		"{\n"
		"    gl_FragColor = Frag_Color;\n"
		"}\n";

	if (fragShaderNaive_ == nullptr) {
		fragShaderNaive_ = std::make_shared<KcGlslShader>(KcGlslShader::k_shader_fragment, frag_shader_naive);
		assert(fragShaderNaive_->compileStatus());
	}

	return fragShaderNaive_;
}


KsShaderManager::shader_ptr KsShaderManager::fragShaderColorUV()
{
	const static char* frag_shader_color_uv =
	    "uniform sampler2D Texture;\n"
		"varying vec2 Frag_UV;\n"
		"varying vec4 Frag_Color;\n"
		"void main()\n"
		"{\n"
		"    gl_FragColor = Frag_Color * texture2D(Texture, Frag_UV.st);\n"
		"}\n";

	if (fragShaderColorUV_ == nullptr) {
		fragShaderColorUV_ = std::make_shared<KcGlslShader>(KcGlslShader::k_shader_fragment, frag_shader_color_uv);
		assert(fragShaderColorUV_->compileStatus());
	}

	return fragShaderColorUV_;
}


KsShaderManager::program_ptr KsShaderManager::programMono()
{
	if (progMono_ == nullptr) {
		progMono_ = std::make_shared<KcGlslProgram>();
		progMono_->attachShader(vertexShaderMono());
		progMono_->attachShader(fragShaderNaive());
		progMono_->link();
		assert(progMono_->linked() && progMono_->linkStatus());
	}

	return progMono_;
}


KsShaderManager::program_ptr KsShaderManager::programColorUV()
{
	if (progColorUV_ == nullptr) {
		progColorUV_ = std::make_shared<KcGlslProgram>();
		progColorUV_->attachShader(vertexShaderColorUV());
		progColorUV_->attachShader(fragShaderColorUV());
		progColorUV_->link();
		assert(progColorUV_->linked() && progColorUV_->linkStatus());
	}

	return progColorUV_;
}