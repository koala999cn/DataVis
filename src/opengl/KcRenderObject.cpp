#include "KcRenderObject.h"
#include "glad.h"
#include "KcGpuBuffer.h"
#include "KcGlslProgram.h"
#include "KcVertexDeclaration.h"


KcRenderObject::KcRenderObject(const KcRenderObject& rhs)
	: type_(rhs.type_)
	, prog_(rhs.prog_)
	, vbos_(rhs.vbos_)
	, ibos_(rhs.ibos_)
	, projMat_(rhs.projMat_)
	, clipBox_(rhs.clipBox_)
	, color_(rhs.color_)
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
		i.buf->bind();
		i.decl->declare();
	}
}


void KcRenderObject::setUniforms_(const std::shared_ptr<KcGlslProgram>& shader) const
{
	// 给shader的uniform赋值
	auto loc = shader->getUniformLocation("matMvp");
	if (loc != -1) {
		if constexpr (decltype(projMat_)::rowMajor())
		    glUniformMatrix4fv(loc, 1, GL_TRUE, projMat_.data());
		else 
			glUniformMatrix4fv(loc, 1, GL_FALSE, projMat_.data());
	}

	loc = shader->getUniformLocation("vClipLower");
	if (loc != -1)
	    glUniform3f(loc, clipBox_.lower().x(), clipBox_.lower().y(), clipBox_.lower().z());
	loc = shader->getUniformLocation("vClipUpper");
	if (loc != -1)
	    glUniform3f(loc, clipBox_.upper().x(), clipBox_.upper().y(), clipBox_.upper().z());

	loc = shader->getUniformLocation("vColor");
	if (loc != -1)
		glUniform4f(loc, color_[0], color_[1], color_[2], color_[3]);
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
			if (!i.decl->hasInstance()) {
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
		for (unsigned j = 0; j < i.decl->attributeCount(); j++) {
			auto& attr = i.decl->getAttribute(j);
			if (attr.semantic() == KcVertexAttribute::k_instance) 
				instances_ = std::max(i.buf->bytesCount() / i.decl->vertexSize() * attr.divisor(), instances_);
		}
	}
}


bool KcRenderObject::hasSemantic(int semantic) const
{
	for (auto& i : vbos_)
		if (i.decl->hasSemantic(semantic))
			return true;

	return false;
}


bool KcRenderObject::hasColor() const
{
	return hasSemantic(KcVertexAttribute::k_diffuse);
}


bool KcRenderObject::hasUV() const
{
	return hasSemantic(KcVertexAttribute::k_texcoord);
}


bool KcRenderObject::hasNormal() const
{
	return hasSemantic(KcVertexAttribute::k_normal);
}


bool KcRenderObject::hasInst() const
{
	return hasSemantic(KcVertexAttribute::k_instance);
}


void KcRenderObject::pushVbo(std::shared_ptr<KcGpuBuffer> vbo, std::shared_ptr<KcVertexDeclaration> vtxDecl)
{
	assert(vbos_.empty() || vtxDecl->getAttribute(0).location() != 0);

	vbos_.push_back({ vbo, vtxDecl });
	calcInst_();
}
