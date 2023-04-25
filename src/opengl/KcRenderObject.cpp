#include "KcRenderObject.h"
#include "glad.h"
#include "KcGpuBuffer.h"
#include "KcGlslProgram.h"
#include "KcVertexDeclaration.h"
#include "KsShaderManager.h"


KcRenderObject::KcRenderObject(const KcRenderObject& rhs)
	: type_(rhs.type_)
	, prog_(rhs.prog_)
	, vbos_(rhs.vbos_)
	, ibos_(rhs.ibos_)
	, projMat_(rhs.projMat_)
	, clipBox_(rhs.clipBox_)
	, color_(rhs.color_)
	, normalMat_(rhs.normalMat_)
	, lightDir_(rhs.lightDir_)
{
}


void KcRenderObject::draw() const
{
	// TODO: 此处没有保存和恢复render状态
	prog_->useProgram(); // 激活shader
	setUniforms_(prog_);
	bindVbo_();
	drawVbo_();
}


void KcRenderObject::bindVbo_() const
{
	for (auto& i : vbos_) {
		if (i.decl && i.buf) {
			i.buf->bind();
			i.decl->declare();
		}
	}
}


void KcRenderObject::setUniforms_(const std::shared_ptr<KcGlslProgram>& shader) const
{
	// 给shader的uniform赋值
	auto& sm = KsShaderManager::singleton();
	shader->setUniform(sm.varname(KsShaderManager::k_mvp_matrix), projMat_);
	if (!hasColor(true))
	    shader->setUniform(sm.varname(KsShaderManager::k_flat_color), color_);

	shader->setUniform(sm.varname(KsShaderManager::k_clip_lower), clipBox_.lower());
	shader->setUniform(sm.varname(KsShaderManager::k_clip_upper), clipBox_.upper());

	if (hasNormal(true)) {
		shader->setUniform(sm.varname(KsShaderManager::k_normal_matrix), normalMat_);
		shader->setUniform(sm.varname(KsShaderManager::k_light_dir), lightDir_);
		shader->setUniform(sm.varname(KsShaderManager::k_light_color), lightColor_);
		shader->setUniform(sm.varname(KsShaderManager::k_ambient_color), ambientColor_);
		shader->setUniform(sm.varname(KsShaderManager::k_specular_color), specularColor_);
		shader->setUniform(sm.varname(KsShaderManager::k_shininess), shininess_);
		shader->setUniform(sm.varname(KsShaderManager::k_eye_pos), eyePos_);
	}
}


void KcRenderObject::drawVbo_() const
{
	const static GLenum glModes[] = {
		GL_POINTS,
		GL_LINES,
		GL_LINE_STRIP,
		GL_LINE_LOOP,
		GL_TRIANGLES,
		GL_TRIANGLE_STRIP,
		GL_TRIANGLE_FAN,
		GL_QUADS,
		GL_QUAD_STRIP,
		GL_POLYGON
	};

	if (!ibos_.empty()) {
		for (auto& i : ibos_) {
			i.buf->bind();
			auto idxSize = i.buf->bytesCount() / i.count;
			assert(idxSize <= 4);
			GLenum type = (idxSize == 4) ? GL_UNSIGNED_INT : (idxSize == 2) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_BYTE;
			if (instances_ == 1)
			    glDrawElements(glModes[i.type], i.count, type, 0);
			else
			    glDrawElementsInstanced(glModes[i.type], i.count, type, 0, instances_);
		}
	}
	else {
		unsigned count(0);
		for (auto& i : vbos_) {
			if (i.decl && !i.decl->hasInstance()) {
				count = i.buf->bytesCount() / i.decl->vertexSize();
				break;
			}
		}

		if (instances_ == 1)
			glDrawArrays(glModes[type_], 0, count);
		else
			glDrawArraysInstanced(glModes[type_], 0, count, instances_);
	}
}


void KcRenderObject::cloneTo_(KcRenderObject& obj) const
{
	obj.type_ = type_;
	obj.prog_ = prog_;
	obj.vbos_ = vbos_;
	obj.ibos_ = ibos_;
	obj.projMat_ = projMat_;
	obj.clipBox_ = clipBox_;
	obj.color_ = color_;
	obj.instances_ = instances_;

	obj.normalMat_ = normalMat_;
	obj.lightDir_ = lightDir_;
	obj.lightColor_ = lightColor_;
	obj.ambientColor_ = ambientColor_;
	obj.specularColor_ = specularColor_;
	obj.shininess_ = shininess_;
}


KcRenderObject* KcRenderObject::clone() const
{
	auto obj = new KcRenderObject(type_);
	cloneTo_(*obj);
	return obj;
}


void KcRenderObject::calcInst_()
{
	instances_ = vbos_.empty() ? 0 : 1; // 初始值

	for (auto& i : vbos_) {
		if (i.buf && i.decl) {
			for (unsigned j = 0; j < i.decl->attributeCount(); j++) {
				auto& attr = i.decl->getAttribute(j);
				if (attr.semantic() == KcVertexAttribute::k_instance)
					instances_ = std::max(i.buf->bytesCount() / i.decl->vertexSize() * attr.divisor(), instances_);
			}
		}
	}
}


bool KcRenderObject::hasAttribute(int semantic, bool enableTest) const
{
	for (auto& i : vbos_) {
		if (i.decl) {
			for (unsigned j = 0; j < i.decl->attributeCount(); j++) {
				auto& attr = i.decl->getAttribute(j);
				if (attr.semantic() == semantic && (!enableTest || attr.enabled()))
					return true;
			}
		}
	}

	return false;
}


bool KcRenderObject::hasColor(bool enableTest) const
{
	return hasAttribute(KcVertexAttribute::k_diffuse, enableTest);
}


bool KcRenderObject::hasUV(bool enableTest) const
{
	return hasAttribute(KcVertexAttribute::k_texcoord, enableTest);
}


bool KcRenderObject::hasNormal(bool enableTest) const
{
	return hasAttribute(KcVertexAttribute::k_normal, enableTest);
}


bool KcRenderObject::hasInst(bool enableTest) const
{
	return hasAttribute(KcVertexAttribute::k_instance, enableTest);
}


void KcRenderObject::pushVbo(std::shared_ptr<KcGpuBuffer> vbo, std::shared_ptr<KcVertexDeclaration> vtxDecl)
{
	assert(vbos_.empty() || vtxDecl->getAttribute(0).location() != 0);

	vbos_.push_back({ vbo, vtxDecl });
	calcInst_();
}


void KcRenderObject::enableAttribute(int semantic, bool b)
{
	for (auto& i : vbos_) {
		if (i.decl) {
			for (unsigned j = 0; j < i.decl->attributeCount(); j++) {
				auto& attr = i.decl->getAttribute(j);
				if (attr.semantic() == semantic)
					attr.enable(b);
			}
		}
	}
}
