#include "KsShaderManager.h"
#include "KcGlslShader.h"
#include "KcGlslProgram.h"
#include "KuStrUtil.h"
#include <assert.h>


std::string KsShaderManager::decorateVertexShader_(const char* source, bool flat, bool hasClipBox)
{
	std::string res(source);
	if (flat) 
		KuStrUtil::replaceSubstr(res, "out vec4 Frag_Color", "flat out vec4 Frag_Color");

	if (hasClipBox) {
		static const char* clip_decl =
			"uniform vec3 vClipLower;\n"
			"uniform vec3 vClipUpper;\n";

		static const char* clip_statement =
			"    gl_ClipDistance[0] = iPosition.x - vClipLower.x;\n"
			"    gl_ClipDistance[1] = iPosition.y - vClipLower.y;\n"
			"    gl_ClipDistance[2] = iPosition.z - vClipLower.z;\n"
			"    gl_ClipDistance[3] = vClipUpper.x - iPosition.x;\n"
			"    gl_ClipDistance[4] = vClipUpper.y - iPosition.y;\n"
			"    gl_ClipDistance[5] = vClipUpper.z - iPosition.z;\n";

		res.insert(0, clip_decl);
		auto pos = res.find_last_of('}');
		res.insert(pos, clip_statement);
	}

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
		"uniform mat4 matMvp;\n"
		"uniform vec4 vColor;\n"
		"in vec3 iPosition;\n"
		"out vec4 Frag_Color;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = matMvp * vec4(iPosition, 1);\n"
		"    Frag_Color = vColor;\n"
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
		"in vec3 iPosition;\n"
		"in vec3 iNormal;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = matMvp * vec4(iPosition, 1);\n"
		"    vec3 vNorm = normalize(matNormal * vec4(iNormal, 0)).xyz;\n"
		"    vec3 vLightDir = vec3(1.0, 1.0, 1.0);\n"
		"    float fDot = max(0.0, dot(vNorm, vLightDir));\n"
		"    Frag_Color.rgb = vColor.rgb * fDot;\n"
		"    Frag_Color.a = vColor.a;\n"
		"}\n";

	return vertex_shader_mono_light;
}


const char* KsShaderManager::fsNavie_()
{
	static const char* frag_shader_navie =
		"in vec4 Frag_Color;\n"
		"void main()\n"
		"{\n"
		"    gl_FragColor = Frag_Color;\n"
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

		if (type & k_frag) {
			type &= k_fs_mask;

			const char* p = fsNavie_();
			if (type & k_uv) 
				p = fsUV_();

			auto src = decorateFragShader_(p, type & k_flat);
			shader = std::make_shared<KcGlslShader>(KcGlslShader::k_shader_fragment, src);
		}
		else {
			type &= k_vs_mask;

			const char* p = vsMono_();
			if (type & k_color) {
				p = vsColor_();
				if (type & k_uv)
					p = vsColorUV_();
			}
			else {
				if (type & k_uv) {
					p = vsUV_();
					if (type & k_instance)
						p = vsInstUV_();
				}
				else if (type & k_instance)
					p = vsInst_();
			}

			auto src = decorateVertexShader_(p, type & k_flat, type & k_clipbox);
			shader = std::make_shared<KcGlslShader>(KcGlslShader::k_shader_vertex, src);
		}
	}

	auto info = shader->infoLog();
	assert(shader->compileStatus());
	return shader;
}


KsShaderManager::program_ptr KsShaderManager::fetchProg(int type, bool flat, bool hasClipBox)
{
	assert(!(type & (k_flat | k_clipbox)));

	if (flat) type |= k_flat;
	if (hasClipBox) type |= k_clipbox;

	auto& prog = progs_[type];

	if (!prog) {
		prog = std::make_shared<KcGlslProgram>();
		prog->attachShader(fetchShader_(type & k_vs_mask));
		prog->attachShader(fetchShader_(type & k_fs_mask | k_frag));
		prog->link();
		auto info = prog->infoLog();
		assert(prog->linked() && prog->linkStatus());
	}

	return prog;
}
