#pragma once
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

	// ��ɫ������ɫ������flatģʽ
	// ���ж���ͬɫ����ɫֵ��uniform����vColorȷ��
	// attr = pos
	// uniform = matMvp + vColor
	shader_ptr vsMono();

	// ��ɫ������ɫ������flat��smoothģʽ
	// ��������ɫ����ɫֵ��attributeȷ��
	// attr = pos + color
	// uniform = matMvp
	shader_ptr vsColor(bool flat);

	// ��ɫ+��������ɫ������flat��smoothģʽ
	// attr = pos + color + uv
	// uniform = matMvp
	shader_ptr vsColorUV(bool flat);

	// ��ʵ�����ƶ�ά����Ķ�����ɫ
	// attr = pos(float2) + offset(float3, instanced)
	// uniform = matMvp(float4x4) + color(float4) + scale(float2)
	shader_ptr vsInst2d();

	// ��ʵ�����ƶ�ά����Ķ�����ɫ
	// attr = pos(float2) + color(float4) + offset(float3, instanced)
	// uniform = matMvp(float4x4) + scale(float2)
	shader_ptr vsInst2dColor();

	// ��ɫ������ɫ������flat��smoothģʽ
	// attr = pos + normal
	// uniform = matMvp + matNormal + vColor
	shader_ptr vsMonoLight(bool flat);

	// ֱͨƬ����ɫ��out-color = in-color
	// ����flat��smoothģʽ
	shader_ptr fsNavie(bool flat);

	// ��ɫ + ����Ƭ����ɫ������flat��smoothģʽ
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

	// �������������ڶ�λprogram��flat�汾
	program_ptr flatVersion(const program_ptr& prog, bool flat);

private:
	KsShaderManager() = default;
	~KsShaderManager() = default;

	KsShaderManager(const KsShaderManager&) = delete;
	void operator=(const KsShaderManager&) = delete;

	// 2����������
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
