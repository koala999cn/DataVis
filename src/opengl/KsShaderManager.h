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

	shader_ptr vertexShaderMono();

	shader_ptr fragShaderNaive();

	program_ptr programMono();

private:
	KsShaderManager();
	~KsShaderManager();

	KsShaderManager(const KsShaderManager&) = delete;
	void operator=(const KsShaderManager&) = delete;


private:
	
	shader_ptr vertexShaderMono_;
	shader_ptr fragShaderNaive_;

	program_ptr progMono_;
};
