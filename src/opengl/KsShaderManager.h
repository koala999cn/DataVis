#pragma once
#include <memory>
#include "KtSingleton.h"

class KcGlslShader;
class KcGlslProgram;

// 封装常用的opengl shaders

class KsShaderManager
{
public:
	using singleton_type = KtSingleton<KsShaderManager, false, true>;
	friend singleton_type;

	static KsShaderManager& singleton() {
		return *singleton_type::instance();
	}

	using shader_ptr = std::shared_ptr<KcGlslShader>;
	using program_ptr = std::shared_ptr<KcGlslProgram>;

	// attr = pos
	// uniform = color
	shader_ptr vertexShaderMono();

	// attr = pos + uv + color
	shader_ptr vertexShaderColorUV();

	// out-color = in-color
	shader_ptr fragShaderNaive();

	// out-color = in-color * tex(uv)
	shader_ptr fragShaderColorUV();

	// vertexShaderMono_ + fragShaderNaive_
	program_ptr programMono();

	// vertexShaderColorUV_ + fragShaderColorUV_
	program_ptr programColorUV();


private:
	KsShaderManager();
	~KsShaderManager();

	KsShaderManager(const KsShaderManager&) = delete;
	void operator=(const KsShaderManager&) = delete;


private:
	
	shader_ptr vertexShaderMono_;
	shader_ptr vertexShaderColorUV_;
	shader_ptr fragShaderNaive_;
	shader_ptr fragShaderColorUV_;

	program_ptr progMono_; 
	program_ptr progColorUV_; 
};
