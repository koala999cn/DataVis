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

	// vsMonoLight(flat) + fsNavie(flat)
	program_ptr progMonoLight(bool flat);

private:
	KsShaderManager();
	~KsShaderManager();

	KsShaderManager(const KsShaderManager&) = delete;
	void operator=(const KsShaderManager&) = delete;

	// 2����������
	static shader_ptr createShader_(int type, const char* source, bool flat);
	static program_ptr createProg_(program_ptr& out, const shader_ptr& vs, const shader_ptr& fs);

private:
	
	shader_ptr vsMono_;
	shader_ptr vsColor_[2];
	shader_ptr vsColorUV_[2];
	shader_ptr vsMonoLight_[2];
	shader_ptr fsNavie_[2];
	shader_ptr fsColorUV_[2];

	program_ptr progMono_;
	program_ptr progColor_[2];
	program_ptr progColorUV_[2]; 
	program_ptr progMonoLight_[2];
};
