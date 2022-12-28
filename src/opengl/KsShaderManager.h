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

	// 所有顶点同色，颜色值由uniform变量vColor确定
	// attr = pos
	// uniform = matMvp + vColor
	shader_ptr vertexShaderFlat();

	// 各顶点异色，颜色值由attribute确定
	// attr = pos + color
	// uniform = matMvp
	shader_ptr vertexShaderSmooth();

	// attr = pos + color + uv
	// uniform = matMvp
	shader_ptr vertexShaderSmoothUV();

	// attr = pos + normal
	// uniform = matMvp + matNormal + vColor
	shader_ptr vertexShaderFlatLight();

	// out-color = in-color
	shader_ptr fragShaderFlat();

	// out-color = in-color * tex(uv)
	shader_ptr fragShaderSmoothUV();

	// vertexShaderFlat + fragShaderFlat
	program_ptr programFlat();

	// vertexShaderSmooth + fragShaderFlat
	program_ptr programSmooth();

	// vertexShaderSmoothUV + fragShaderSmoothUV
	program_ptr programSmoothUV();

	// vertexShaderFlatLight + fragShaderFlat
	program_ptr programFlatLight();

private:
	KsShaderManager();
	~KsShaderManager();

	KsShaderManager(const KsShaderManager&) = delete;
	void operator=(const KsShaderManager&) = delete;


private:
	
	shader_ptr vertexShaderFlat_;
	shader_ptr vertexShaderSmooth_;
	shader_ptr vertexShaderSmoothUV_;
	shader_ptr vertexShaderFlatLight_;
	shader_ptr fragShaderFlat_;
	shader_ptr fragShaderSmoothUV_;

	program_ptr progFlat_; 
	program_ptr progSmooth_;
	program_ptr progSmoothUV_; 
	program_ptr progFlatLight_;
};
