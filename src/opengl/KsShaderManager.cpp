#include "KsShaderManager.h"
#include "KcGlslShader.h"
#include "KcGlslProgram.h"
#include "KuStrUtil.h"
#include <assert.h>
#include <sstream>


KsShaderManager::KsShaderManager()
{
	names_.resize(k_variant_count);

	names_[k_world_matrix] = "k_WorldMatrix";
	names_[k_model_view_matrix] = "k_ModelViewMatrix";
	names_[k_proj_matrix] = "k_ProjMatrix";
	names_[k_mvp_matrix] = "k_MvpMatrix";
	names_[k_normal_matrix] = "k_NormalMatrix";

	names_[k_clip_lower] = "k_ClipLower";
	names_[k_clip_upper] = "k_ClipUpper";
	
	names_[k_eye_pos] = "k_EyePos";
	names_[k_light_dir] = "k_LightDir";
	names_[k_light_color] = "k_LightColor";
	names_[k_ambient_color] = "k_AmbientColor";
	names_[k_specular_color] = "k_SpecularColor";
	names_[k_shininess] = "k_Shininess";

	names_[k_flat_color] = "k_FlatColor";

	names_[k_vertex_position] = "k_inPosition";
	names_[k_vertex_normal] = "k_inNormal";
	names_[k_vertex_color] = "k_inColor";
	names_[k_vertex_secondary_color] = "k_inSecondaryColor";
	names_[k_vertex_fog_coord] = "k_inFogCoord";
	names_[k_vertex_tex_coord0] = "k_inUV";

	names_[k_vs_out_position] = "gl_Position";
	names_[k_vs_out_color] = "k_VertColor";
	names_[k_vs_out_tex_coord0] = "k_VertUV";

	names_[k_fs_out_color] = "gl_FragColor";
}


std::string KsShaderManager::decorateVertexShader_(const char* source, bool flat, bool hasClipBox)
{
	std::string res(source);
	if (flat) 
		KuStrUtil::replaceSubstr(res, "out vec4 Frag_Color", "flat out vec4 Frag_Color");

	res.insert(0, "#version 330 core\n");
	return res;
}


std::string KsShaderManager::decorateFragShader_(const char* source, bool flat)
{
	std::string res(source);

	if (flat)
		KuStrUtil::replaceSubstr(res, "in vec4 Frag_Color", "flat in vec4 Frag_Color");

	res.insert(0, "#version 330 core\n");
	return res;
}



const char* KsShaderManager::vsMono_()
{
	static const char* vertex_shader_mono =
		"uniform mat4 k_MvpMatrix;\n"
		"uniform vec4 k_FlatColor;\n"
		"in vec3 k_inPosition;\n"
		"out vec4 k_VertColor;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = k_MvpMatrix * vec4(k_inPosition, 1);\n"
		"    k_VertColor = k_FlatColor;\n"
		"}\n";

	return vertex_shader_mono;
}


const char* KsShaderManager::vsColor_()
{
	static const char* vertex_shader_color =
		"out vec4 Frag_Color;\n"
		"uniform mat4 matMvp;\n"
		"in vec3 iPosition;\n"
		"in vec4 iColor;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = matMvp * vec4(iPosition, 1);\n"
		"    Frag_Color = iColor;\n"
		"}\n";

	return vertex_shader_color;
}


const char* KsShaderManager::vsUV_()
{
	static const char* vertex_shader_uv =
		"out vec2 Frag_UV;\n"
		"out vec4 Frag_Color;\n"
		"uniform mat4 matMvp;\n"
		"uniform vec4 vColor;\n"
		"layout (location = 0) in vec3 iPosition;\n"
		"layout (location = 1) in vec2 iUV;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = matMvp * vec4(iPosition, 1);\n"
		"    Frag_UV = iUV;\n"
		"    Frag_Color = vColor;\n"
		"}\n";

	return vertex_shader_uv;
}


const char* KsShaderManager::vsColorUV_()
{
	static const char* vertex_shader_color_uv =
		"out vec4 Frag_Color;\n"
		"out vec2 Frag_UV;\n"
		"uniform mat4 matMvp;\n"
		"layout (location = 0) in vec3 iPosition;\n"
		"layout (location = 1) in vec2 iUV;\n"
		"layout (location = 2) in vec4 iColor;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = matMvp * vec4(iPosition, 1);\n"
		"    Frag_UV = iUV;\n"
		"    Frag_Color = iColor;\n"
		"}\n";

	return vertex_shader_color_uv;
}


const char* KsShaderManager::vsInst_()
{
	// @vColor用于fill，@vSecondaryColor用于outline
	// @iVertex: marker的标准顶点坐标, w值用于标记是fill还是outline顶点
	// @iPosition: 当前instance的位置坐标
	// @iColor: 当前instance的fill色（可使每个instance的颜色不同）
	// @iSize: 当前instance的缩放因子（可使每个instance的大小不同）
	// @bVaryingColor: 是否启用iColor属性
	// @bVaryingSize: 是否启用iSize属性
	static const char* vertex_shader_inst =
		"uniform mat4 matMvp;\n"
		"uniform vec4 vColor;\n"
		"uniform vec4 vSecondaryColor;\n"
		"uniform vec3 vScale;\n"
		"uniform int bColorVarying;\n"
		"uniform int bSizeVarying;\n"
		"layout (location = 0) in vec4 iVertex;\n"
		"layout (location = 1) in vec3 iPosition;\n"
		"layout (location = 2) in float iSize;\n"
		"layout (location = 3) in vec4 iColor;\n"
		"out vec4 Frag_Color;\n"
		"void main()\n"
		"{\n"
		"    vec3 v = iVertex.xyz * vScale;\n"
		"    if (bSizeVarying != 0) v *= iSize;\n"
		"    gl_Position = matMvp * vec4(iPosition, 1) + vec4(v, 0);\n"
		"    if (iVertex.w != 0) Frag_Color = vSecondaryColor;\n"
		"    else if (bColorVarying != 0) Frag_Color = iColor;\n"
		"    else Frag_Color = vColor;\n"
		"}\n";

	return vertex_shader_inst;
}


const char* KsShaderManager::vsInstUV_()
{
	// NB： 仅适用于绘制quad对象（如text）
	// 
	// @iVertex: 用于区分quad的4个顶点
	// @vScale: 用于将screen坐标变换到NDC坐标
	// @iPosition: 相当于各实例的anchor点
	// @iOffset: dx0, dy0, dx1, dy1，均为屏幕坐标尺寸
	// @iUVs: u0, v0, u1, v1
	static const char* vertex_shader_inst_uv =
		"uniform mat4 matMvp;\n"
		"uniform vec4 vColor;\n"
		"uniform vec3 vScale;\n"
		"uniform int bColorVarying;\n"
		"uniform int bSizeVarying;\n"
		"layout (location = 0) in vec4 iVertex;\n"
		"layout (location = 1) in vec3 iPosition;\n"
		"layout (location = 2) in vec4 iOffset;\n"
		"layout (location = 3) in vec4 iUVs;\n"
		"layout (location = 4) in float iSize;\n"
		"layout (location = 5) in vec4 iColor;\n"
		"out vec4 Frag_Color;\n"
		"out vec2 Frag_UV;\n"
		"void main()\n"
		"{\n"
		"    Frag_UV.x = dot(iVertex.xz, iUVs.xz);\n"
		"    Frag_UV.y = dot(iVertex.yw, iUVs.yw);\n"
		"    vec2 offset;\n "
		"    offset.x = dot(iVertex.xz, iOffset.xz) * vScale.x;\n"
		"    offset.y = dot(iVertex.yw, iOffset.yw) * vScale.y;\n"
		// TODO: "    if (bSizeVarying != 0) v *= iSize;\n"
		"    gl_Position = matMvp * vec4(iPosition, 1) + vec4(offset, 0, 0);\n"
		"    if (bColorVarying != 0) Frag_Color = iColor;\n"
		"    else Frag_Color = vColor;\n"
		"}\n";

	return vertex_shader_inst_uv;
}


const char* KsShaderManager::vsMonoLight_()
{
	static const char* vertex_shader_mono_light =
		"out vec4 Frag_Color;\n"
		"uniform mat4 matMvp;\n"
		"uniform mat4 matNormal;\n"
		"uniform vec4 vColor;\n"
		"uniform vec3 vLightDir;\n"
		"layout (location = 0) in vec3 iPosition;\n"
		"layout (location = 1) in vec3 iNormal;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = matMvp * vec4(iPosition, 1);\n"
		"    vec3 vNorm = normalize((matNormal * vec4(iNormal, 0)).xyz);\n"
		"    float fDot = max(0.0, dot(vNorm, -vLightDir));\n"
	//	"    float fDot = max(0.0, dot(iNormal, -vLightDir));\n"
		"    Frag_Color.rgb = vColor.rgb * fDot;\n"
		"    Frag_Color.a = vColor.a;\n"
		"}\n";

	return vertex_shader_mono_light;
}


const char* KsShaderManager::vsColorLight_()
{
	static const char* vertex_shader_color_light =
		"out vec4 Frag_Color;\n"
		"uniform mat4 matMvp;\n"
		"uniform mat4 matNormal;\n"
		"uniform vec3 vLightDir;\n"
		"uniform vec3 vAmbientColor;\n"
		"uniform vec3 vLightColor;\n"
		"uniform vec3 vEyePos;\n"
		"uniform vec3 vSpecularIntensity;\n"
		"uniform float fShininess;\n"
		"layout (location = 0) in vec3 iPosition;\n"
		"layout (location = 1) in vec4 iColor;\n"
		"layout (location = 2) in vec3 iNormal;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = matMvp * vec4(iPosition, 1);\n"
		"    vec3 vNorm = normalize((matNormal * vec4(iNormal, 0)).xyz);\n"
		"    float fDot = max(0.0, dot(vNorm, -vLightDir));\n"
		"    vec3 diffuse = vLightColor * fDot;\n"
		"    vec3 eyeDir = normalize(vEyePos - iPosition);\n"
		"    vec3 reflectDir = normalize(reflect(-vLightDir, iNormal));\n"
		"    float spec = pow(max(dot(eyeDir, reflectDir), 0.0), fShininess);\n"
		"    vec3 specular = spec * vSpecularIntensity * vLightColor;\n"
		"    Frag_Color.rgb = min(iColor.rgb * (vAmbientColor + diffuse + specular), vec3(1, 1, 1));\n"
		"    Frag_Color.a = iColor.a;\n"
		"}\n";

	return vertex_shader_color_light;
}


const char* KsShaderManager::fsNavie_()
{
	static const char* frag_shader_navie =
		"in vec4 k_VertColor;\n"
		"void main()\n"
		"{\n"
		"    gl_FragColor = k_VertColor;\n"
		"}\n";

	return frag_shader_navie;
}


const char* KsShaderManager::fsUV_()
{
	static const char* frag_shader_color_uv =
		"in vec4 Frag_Color;\n"
		"in vec2 Frag_UV;\n"
	    "uniform sampler2D Texture;\n"	
		"void main()\n"
		"{\n"
		"    gl_FragColor = Frag_Color * texture2D(Texture, Frag_UV.st);\n"
		"}\n";

	return frag_shader_color_uv;
}


KsShaderManager::shader_ptr KsShaderManager::fetchShader_(int type)
{
	auto& shader = shaders_[type];

	if (!shader) { 
		auto shaderType = KcGlslShader::k_shader_vertex;
		std::string source("#version 330 core\n");
		
		if (type & k_frag) {
			shaderType = KcGlslShader::k_shader_fragment;
			source += fsDecls_(type);
		}
		else {
			source += vsDecls_(type);
		}

		source += "void main()\n{\n";
		if (type & k_frag) 
			source += fsBody_(type);
		else 
			source += vsBody_(type);
		source += "}\n";

		shader = std::make_shared<KcGlslShader>(shaderType, source);
	}

	auto info = shader->infoLog();
	assert(shader->compileStatus());
	return shader;
}


std::string KsShaderManager::vsDecls_(int type) const
{
	int loc(0);
	std::string decls;

	decls += declUniform_("mat4", names_[k_mvp_matrix]);

	decls += layoutPrefix_(loc++);
	decls += declAttribute_("vec3", names_[k_vertex_position]);

	if (type & k_flat)
		decls += "flat ";
	decls += "out vec4 " + names_[k_vs_out_color] + ";\n";

	if (type & k_uv) {
		decls += layoutPrefix_(loc++);
		decls += declAttribute_("vec2", names_[k_vertex_tex_coord0]);

		decls += "out vec2 " + names_[k_vs_out_tex_coord0] + ";\n";
	}

	if (type & k_color) {
		decls += layoutPrefix_(loc++);
		decls += declAttribute_("vec4", names_[k_vertex_color]);
	}
	else { // k_mono
		decls += declUniform_("vec4", names_[k_flat_color]);
	}

	if (type & k_normal) {
		decls += layoutPrefix_(loc++);
		decls += declAttribute_("vec3", names_[k_vertex_normal]); 

		decls += declUniform_("mat4", names_[k_normal_matrix]);
		decls += declUniform_("vec3", names_[k_light_dir]);
		decls += declUniform_("vec3", names_[k_ambient_color]);
		decls += declUniform_("vec3", names_[k_light_color]);
		decls += declUniform_("vec3", names_[k_eye_pos]);
		decls += declUniform_("vec3", names_[k_specular_color]);
		decls += declUniform_("float", names_[k_shininess]);
	}

	if (type & k_clipbox) {
		decls += declUniform_("vec3", names_[k_clip_lower]);
		decls += declUniform_("vec3", names_[k_clip_upper]);
	}

	return decls;
}


std::string KsShaderManager::layoutPrefix_(int loc)
{
	return "layout (location = " + std::to_string(loc) + ") ";
}


std::string KsShaderManager::declUniform_(const std::string_view& type, const std::string_view& name)
{
	std::string decl("uniform ");
	decl += type; decl += " ";
	decl += name; decl += ";\n";
	return decl;
}


std::string KsShaderManager::declAttribute_(const std::string_view& type, const std::string_view& name)
{
	std::string decl("in ");
	decl += type; decl += " ";
	decl += name; decl += ";\n";
	return decl;
}


std::string KsShaderManager::vsBody_(int type) const
{
	std::ostringstream body;

	body << "\t" << names_[k_vs_out_position] <<" = ";
	body << names_[k_mvp_matrix] << " * vec4(" << names_[k_vertex_position] << ", 1);\n";

	if (type & k_color) {
		body << "\t" << names_[k_vs_out_color] << " = " << names_[k_vertex_color] << ";\n";
	}
	else { // k_mono
		body << "\t" << names_[k_vs_out_color] << " = " << names_[k_flat_color] << ";\n";
	}

	if (type & k_uv)
		body << "\t" << names_[k_vs_out_tex_coord0] << " = " << names_[k_vertex_tex_coord0] << ";\n";

	if (type & k_clipbox) {
		auto& vpos = names_[k_vertex_position];
		auto& lo = names_[k_clip_lower];
		auto& up = names_[k_clip_upper];

		body << "\tgl_ClipDistance[0] = " << vpos << ".x - " << lo << ".x;\n";
		body << "\tgl_ClipDistance[1] = " << vpos << ".y - " << lo << ".y;\n";
		body << "\tgl_ClipDistance[2] = " << vpos << ".z - " << lo << ".z;\n";
		body << "\tgl_ClipDistance[3] = " << up << ".x - " << vpos << ".x;\n";
		body << "\tgl_ClipDistance[4] = " << up << ".y - " << vpos << ".y;\n";
		body << "\tgl_ClipDistance[5] = " << up << ".z - " << vpos << ".z;\n";
	}

	return body.str();
}


std::string KsShaderManager::fsDecls_(int type) const
{
	std::string decls;
	if (type & k_flat)
		decls += "flat ";
	decls += declAttribute_("vec4", names_[k_vs_out_color]);

	if (type & k_vertex_tex_coord0) {
		decls += declAttribute_("vec2", names_[k_vs_out_tex_coord0]);
		decls += declUniform_("sampler2D", "Texture");
	}

	return decls;
}


std::string KsShaderManager::fsBody_(int type) const
{
	std::string body = "    " + names_[k_fs_out_color] + " = ";
	body += names_[k_vs_out_color] + ";\n"; // TODO: 此非attrib

	if (type & k_uv) {
		body += "    " + names_[k_fs_out_color] + " *= texture2D(Texture, ";
		body += names_[k_vs_out_tex_coord0] + ".st);\n"; // TODO: 此非attrib
	}

	return body;
}


KsShaderManager::program_ptr KsShaderManager::fetchProg(int type)
{
	assert(!(type & k_frag));

	auto& prog = progs_[type];

	if (!prog) {
		prog = std::make_shared<KcGlslProgram>();
		prog->attachShader(fetchShader_(type));
		prog->attachShader(fetchShader_(type | k_frag));
		prog->link();
		auto info = prog->infoLog();
		assert(prog->linked() && prog->linkStatus());
	}

	return prog;
}
