#pragma once
#include <map>
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

	enum KeType
	{
		k_mono      = 0x0000,
		k_color     = 0x0001,
		k_uv        = 0x0002,
		k_normal    = 0x0004,

		k_instance  = 0x0010,

		k_flat      = 0x1000,
		k_clipbox   = 0x2000,
		k_frag      = 0x8000, // 用以区分vs和fs

		k_vs_mask   = k_color | k_uv | k_normal | k_instance | k_flat | k_clipbox,
		k_fs_mask   = k_color | k_uv | k_flat,
		k_prog_mask = k_vs_mask
	};

	// assert(!(type & (k_flat | k_clipbox)));
	program_ptr fetchProg(int type, bool flat, bool hasClipBox);

	// vsMono() + fsNavie(true)
	program_ptr progMono(bool hasClipBox) {
		return fetchProg(k_mono, true, hasClipBox);
	}

	// vsColor(flat) + fsNavie(flat)
	program_ptr progColor(bool flat, bool hasClipBox) {
		return fetchProg(k_color, flat, hasClipBox);
	}

	program_ptr progUV(bool hasClipBox) {
		return fetchProg(k_uv, true, hasClipBox);
	}

	// vsColorUV(flat) + fsColorUV(flat)
	program_ptr progColorUV(bool flat, bool hasClipBox) {
		return fetchProg(k_color | k_uv, flat, hasClipBox);
	}

	// vsMonoInst() + fsNavie(true)
	program_ptr progMonoInst(bool hasClipBox) {
		return fetchProg(k_mono | k_instance, true, hasClipBox);
	}

	// vsColorInst() + fsNavie(true)
	program_ptr progColorInst(bool flat, bool hasClipBox) {
		return fetchProg(k_color | k_instance, flat, hasClipBox);
	}

	// vsMonoLight(flat) + fsNavie(flat)
	program_ptr progMonoLight(bool flat, bool hasClipBox) {
		return fetchProg(k_normal, flat, hasClipBox);
	}


private:
	KsShaderManager() = default;
	~KsShaderManager() = default;

	KsShaderManager(const KsShaderManager&) = delete;
	void operator=(const KsShaderManager&) = delete;

	// 单色顶点着色，总是flat模式
	// 所有顶点同色，颜色值由uniform变量vColor确定
	// attr = pos
	// uniform = matMvp + vColor
	static const char* vsMono_();

	// 多色顶点着色，区分flat和smooth模式
	// 各顶点异色，颜色值由attribute确定
	// attr = pos + color
	// uniform = matMvp
	static const char* vsColor_();

	// 纹理顶点着色
	// attr = pos + uv
	// uniform = matMvp
	static const char* vsUV_();

	// 多色+纹理顶点着色，区分flat和smooth模式
	// attr = pos + color + uv
	// uniform = matMvp
	static const char* vsColorUV_();

	// 多实例绘制的单色顶点着色
	// attr = pos(float3) + offset(float3, instanced)
	// uniform = matMvp(float4x4) + color(float4) + scale(float3)
	static const char* vsInst_();

	// 单色光照着色，区分flat和smooth模式
	// attr = pos + normal
	// uniform = matMvp + matNormal + vColor
	static const char* vsMonoLight_();

	// 直通片段着色，out-color = in-color
	// 区分flat和smooth模式
	static const char* fsNavie_();

	// 纹理片段着色
	// out-color = tex(uv)
	static const char* fsUV_();

	// 多色 + 纹理片段着色，区分flat和smooth模式
	// out-color = in-color * tex(uv)
	static const char* fsColorUV_();

	shader_ptr fetchShader_(int type);

	// 根据flat和hasClipBox参数，装饰改传入的vs代码source，并返回修改后的结果
	static std::string decorateVertexShader_(const char* source, bool flat, bool hasClipBox);
	static std::string decorateFragShader_(const char* source, bool flat);

private:
	std::map<int, shader_ptr> shaders_;
	std::map<int, program_ptr> progs_;
};
