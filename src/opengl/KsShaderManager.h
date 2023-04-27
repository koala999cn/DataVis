#pragma once
#include <map>
#include <vector>
#include <string>
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
		k_matrix    = 0x0008, // inst��Ⱦ����

		k_instance  = 0x0010,

		k_flat      = 0x1000,
		k_clipbox   = 0x2000,

		k_frag      = 0x8000, // �ڲ�ʹ�ã���������vs��fs
	};

	// @type: KeType�����
	program_ptr fetchProg(int type);

	// vsMono() + fsNavie(true)
	program_ptr progMono(bool hasClipBox) {
		return fetchProg(combineType_(k_mono, true, hasClipBox));
	}

	// vsColor(flat) + fsNavie(flat)
	program_ptr progColor(bool flat, bool hasClipBox) {
		return fetchProg(combineType_(k_color, flat, hasClipBox));
	}

	program_ptr progUV(bool hasClipBox) {
		return fetchProg(combineType_(k_uv, true, hasClipBox));
	}

	// vsColorUV(flat) + fsColorUV(flat)
	program_ptr progColorUV(bool flat, bool hasClipBox) {
		return fetchProg(combineType_(k_color | k_uv, flat, hasClipBox));
	}

	// vsMonoInst() + fsNavie(true)
	program_ptr progMonoInst(bool hasClipBox) {
		return fetchProg(combineType_(k_mono | k_instance, true, hasClipBox));
	}

	// vsColorInst() + fsNavie(true)
	program_ptr progColorInst(bool flat, bool hasClipBox) {
		return fetchProg(combineType_(k_color | k_instance, flat, hasClipBox));
	}

	// vsMonoLight(flat) + fsNavie(flat)
	program_ptr progMonoLight(bool flat, bool hasClipBox) {
		return fetchProg(combineType_(k_normal, flat, hasClipBox));
	}

	// ȱʡ��shader����
	enum KeVariant
	{
		/// standard uniforms

		k_world_matrix,
		k_model_view_matrix,
		k_proj_matrix,
		k_mvp_matrix,
		k_normal_matrix,

		k_major_color, // ���ж��㹲�õ���ɫ����mono����ʱʹ��
		k_minor_color, // ���ж��㹲�õĸ�ɫ��һ�����ڻ���outline

		k_clip_lower,
		k_clip_upper,

		// ��ʵ����Ⱦuniforms
		k_inst_scale,
		k_inst_color_varying,
		k_inst_size_varying,

		// ����uniforms
		k_eye_pos,
		k_light_dir,
		k_light_color,
		k_ambient_color,
		k_specular_color,
		k_shininess,

		k_uniform_first = k_world_matrix,
		k_uniform_last = k_shininess,

		/// standard vertex attributes

		k_vertex_position = k_uniform_last + 1,
		k_vertex_normal,
		k_vertex_color,
		k_vertex_secondary_color,
		k_inst_vertex, // ����ʵ���Ķ�������(vec4)��w��Ա��������ʵ����������
		k_inst_size, 
		k_inst_offset,
		k_vertex_fog_coord,
		k_vertex_tex_coord0,

		k_attribute_first = k_vertex_position,
		k_attribute_last = k_vertex_tex_coord0,

		/// vs��fs֮��Ľ�������������Ĭ�ϵ�ȫ�ֱ���

		k_vs_out_position = k_attribute_last + 1,
		k_vs_out_color,
		k_vs_out_tex_coord0,

		k_fs_out_color,

		k_variant_count
	};

	auto& varname(KeVariant var) const { return names_[var]; }

private:
	KsShaderManager();
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
	// attr = pos(float3) + offset(float3, instanced) + color(float4, instanced) + scale(float, instanced)
	// uniform = matMvp(float4x4) + color(float4) + scale(float3)
	static const char* vsInst_();

	static const char* vsInstUV_();

	// ��ɫ������ɫ������flat��smoothģʽ
	// attr = pos + normal
	// uniform = matMvp + matNormal + vColor
	static const char* vsMonoLight_();

	// ��ɫ������ɫ������flat��smoothģʽ
	// attr = pos + color + normal
	// uniform = matMvp + matNormal
	static const char* vsColorLight_();

	// ֱͨƬ����ɫ��out-color = in-color
	// ����flat��smoothģʽ
	static const char* fsNavie_();

	// ����Ƭ����ɫ
	// out-color = in-color * tex(uv)
	static const char* fsUV_();

	shader_ptr fetchShader_(int type);

	// ����flat��hasClipBox������װ�θĴ����vs����source���������޸ĺ�Ľ��
	static std::string decorateVertexShader_(const char* source, bool flat, bool hasClipBox);
	static std::string decorateFragShader_(const char* source, bool flat);

	static int combineType_(int baseType, bool flat, bool hasClipBox) {
		if (flat) baseType |= k_flat;
		if (hasClipBox) baseType |= k_clipbox;
		return baseType;
	}

	std::string vsDecls_(int type) const;
	std::string vsBody_(int type) const;

	std::string fsDecls_(int type) const;
	std::string fsBody_(int type) const;

	static std::string declUniform_(const std::string_view& type, const std::string_view& name);
	static std::string declAttribute_(const std::string_view& type, const std::string_view& name);
	static std::string layoutPrefix_(int loc);

private:
	std::map<int, shader_ptr> shaders_;
	std::map<int, program_ptr> progs_;
	std::vector<std::string> names_; // KeVariant -> ������
};
