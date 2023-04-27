#pragma once
#include <map>
#include <vector>
#include <string>
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
		k_matrix    = 0x0008, // inst渲染独用

		k_instance  = 0x0010,

		k_flat      = 0x1000,
		k_clipbox   = 0x2000,

		k_frag      = 0x8000, // 内部使用，用以区分vs和fs
	};

	// @type: KeType的组合
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

	// 缺省的shader变量
	enum KeVariant
	{
		/// standard uniforms

		k_world_matrix,
		k_model_view_matrix,
		k_proj_matrix,
		k_mvp_matrix,
		k_normal_matrix,

		k_major_color, // 所有顶点共用的主色，仅mono绘制时使用
		k_minor_color, // 所有顶点共用的辅色，一般用于绘制outline

		k_clip_lower,
		k_clip_upper,

		// 多实例渲染uniforms
		k_inst_scale,
		k_inst_color_varying,
		k_inst_size_varying,

		// 光照uniforms
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
		k_inst_vertex, // 单个实例的顶点属性(vec4)，w成员用于区分实例顶点的类别
		k_inst_size, 
		k_inst_offset,
		k_vertex_fog_coord,
		k_vertex_tex_coord0,

		k_attribute_first = k_vertex_position,
		k_attribute_last = k_vertex_tex_coord0,

		/// vs和fs之间的交换变量，或者默认的全局变量

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
	// attr = pos(float3) + offset(float3, instanced) + color(float4, instanced) + scale(float, instanced)
	// uniform = matMvp(float4x4) + color(float4) + scale(float3)
	static const char* vsInst_();

	static const char* vsInstUV_();

	// 单色光照着色，区分flat和smooth模式
	// attr = pos + normal
	// uniform = matMvp + matNormal + vColor
	static const char* vsMonoLight_();

	// 单色光照着色，区分flat和smooth模式
	// attr = pos + color + normal
	// uniform = matMvp + matNormal
	static const char* vsColorLight_();

	// 直通片段着色，out-color = in-color
	// 区分flat和smooth模式
	static const char* fsNavie_();

	// 纹理片段着色
	// out-color = in-color * tex(uv)
	static const char* fsUV_();

	shader_ptr fetchShader_(int type);

	// 根据flat和hasClipBox参数，装饰改传入的vs代码source，并返回修改后的结果
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
	std::vector<std::string> names_; // KeVariant -> 变量名
};
