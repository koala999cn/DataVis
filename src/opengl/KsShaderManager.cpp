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
		"uniform vec4 vColor;\n"
		"in vec3 position;\n"
		"out vec4 Frag_Color;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = mvpMat * vec4(position, 1);\n"
		"    Frag_Color = vColor;\n"
		"}\n";

	if (vertexShaderMono_ == nullptr) {
		vertexShaderMono_ = std::make_shared<KcGlslShader>(KcGlslShader::k_shader_vertex, vertex_shader_mono);
		assert(vertexShaderMono_->compileStatus());
	}

	return vertexShaderMono_;
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