#pragma once
#include <map>
#include <memory>
#include "KtSingleton.h"

class KcGlslShader;
class KcGlslProgram;

// ��װ���õ�opengl shaders

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
		k_frag      = 0x8000, // ��������vs��fs

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

	// ��ɫ������ɫ������flatģʽ
	// ���ж���ͬɫ����ɫֵ��uniform����vColorȷ��
	// attr = pos
	// uniform = matMvp + vColor
	static const char* vsMono_();

	// ��ɫ������ɫ������flat��smoothģʽ
	// ��������ɫ����ɫֵ��attributeȷ��
	// attr = pos + color
	// uniform = matMvp
	static const char* vsColor_();

	// ��������ɫ
	// attr = pos + uv
	// uniform = matMvp
	static const char* vsUV_();

	// ��ɫ+��������ɫ������flat��smoothģʽ
	// attr = pos + color + uv
	// uniform = matMvp
	static const char* vsColorUV_();

	// ��ʵ�����Ƶĵ�ɫ������ɫ
	// attr = pos(float3) + offset(float3, instanced)
	// uniform = matMvp(float4x4) + color(float4) + scale(float3)
	static const char* vsInst_();

	// ��ɫ������ɫ������flat��smoothģʽ
	// attr = pos + normal
	// uniform = matMvp + matNormal + vColor
	static const char* vsMonoLight_();

	// ֱͨƬ����ɫ��out-color = in-color
	// ����flat��smoothģʽ
	static const char* fsNavie_();

	// ����Ƭ����ɫ
	// out-color = tex(uv)
	static const char* fsUV_();

	// ��ɫ + ����Ƭ����ɫ������flat��smoothģʽ
	// out-color = in-color * tex(uv)
	static const char* fsColorUV_();

	shader_ptr fetchShader_(int type);

	// ����flat��hasClipBox������װ�θĴ����vs����source���������޸ĺ�Ľ��
	static std::string decorateVertexShader_(const char* source, bool flat, bool hasClipBox);
	static std::string decorateFragShader_(const char* source, bool flat);

private:
	std::map<int, shader_ptr> shaders_;
	std::map<int, program_ptr> progs_;
};
