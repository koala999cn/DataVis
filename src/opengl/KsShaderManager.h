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

	// 单色顶点着色，总是flat模式
	// 所有顶点同色，颜色值由uniform变量vColor确定
	// attr = pos
	// uniform = matMvp + vColor
	shader_ptr vsMono();

	// 多色顶点着色，区分flat和smooth模式
	// 各顶点异色，颜色值由attribute确定
	// attr = pos + color
	// uniform = matMvp
	shader_ptr vsColor(bool flat);

	// 多色+纹理顶点着色，区分flat和smooth模式
	// attr = pos + color + uv
	// uniform = matMvp
	shader_ptr vsColorUV(bool flat);

	// 多实例绘制二维对象的顶点着色
	// attr = pos(float2) + offset(float3, instanced)
	// uniform = matMvp(float4x4) + color(float4) + scale(float2)
	shader_ptr vsInst2d();

	// 多实例绘制二维对象的顶点着色
	// attr = pos(float2) + color(float4) + offset(float3, instanced)
	// uniform = matMvp(float4x4) + scale(float2)
	shader_ptr vsInst2dColor();

	// 单色光照着色，区分flat和smooth模式
	// attr = pos + normal
	// uniform = matMvp + matNormal + vColor
	shader_ptr vsMonoLight(bool flat);

	// 直通片段着色，out-color = in-color
	// 区分flat和smooth模式
	shader_ptr fsNavie(bool flat);

	// 多色 + 纹理片段着色，区分flat和smooth模式
	// out-color = in-color * tex(uv)
	shader_ptr fsColorUV(bool flat);

	// vsMono() + fsNavie(true)
	program_ptr progMono();

	// vsColor(flat) + fsNavie(flat)
	program_ptr progColor(bool flat);

	// vsColorUV(flat) + fsColorUV(flat)
	program_ptr progColorUV(bool flat);

	// vsInst2d() + fsNavie(true)
	program_ptr progInst2d();

	// vsInst2dColor() + fsNavie(true)
	program_ptr progInst2dColor();

	// vsMonoLight(flat) + fsNavie(flat)
	program_ptr progMonoLight(bool flat);

	// 辅助函数，用于定位program的flat版本
	program_ptr flatVersion(const program_ptr& prog, bool flat);

private:
	KsShaderManager() = default;
	~KsShaderManager() = default;

	KsShaderManager(const KsShaderManager&) = delete;
	void operator=(const KsShaderManager&) = delete;

	// 2个帮助函数
	static shader_ptr createShader_(int type, const char* source, bool flat);
	static program_ptr createProg_(program_ptr& out, const shader_ptr& vs, const shader_ptr& fs);

private:
	
	shader_ptr vsMono_;
	shader_ptr vsColor_[2];
	shader_ptr vsColorUV_[2];
	shader_ptr vsInst2d_;
	shader_ptr vsInst2dColor_;
	shader_ptr vsMonoLight_[2];
	shader_ptr fsNavie_[2];
	shader_ptr fsColorUV_[2];

	program_ptr progMono_;
	program_ptr progColor_[2];
	program_ptr progColorUV_[2]; 
	program_ptr progInst2d_;
	program_ptr progInst2dColor_;
	program_ptr progMonoLight_[2];
};
