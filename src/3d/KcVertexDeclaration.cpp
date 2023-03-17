#include "KcVertexDeclaration.h"
#include "glad.h"
#include <assert.h>


void KcVertexDeclaration::pushAttribute(KcVertexAttribute::KeFormat fmt, KcVertexAttribute::KeSemantic semantic, unsigned semanticIndex)
{
	pushAttribute(KcVertexAttribute(attributeCount(), fmt, vertexSize(), 
		semantic, semanticIndex));
}


const KcVertexAttribute* KcVertexDeclaration::findAttribute(KcVertexAttribute::KeSemantic semantic, unsigned semanticIdx) const
{
	for(unsigned i = 0; i < attributeCount(); i++) {
		const auto& attr = getAttribute(i);
		if(attr.semantic() == semantic && attr.semanticIndex() == semanticIdx)
			return &attr;
	}

	return nullptr;
}


unsigned KcVertexDeclaration::vertexSize() const
{
	unsigned s(0);
	for (unsigned i = 0; i < attributeCount(); i++)
		s += getAttribute(i).byteSize();

	return s;
}


bool KcVertexDeclaration::hasSemantic(KcVertexAttribute::KeSemantic semantic) const
{
	for(unsigned i = 0; i < attributeCount(); i++)
		if(getAttribute(i).semantic() == semantic)
			return true;

	return false;
}


bool KcVertexDeclaration::hasColor() const
{
	for(unsigned i = 0; i < attributeCount(); i++) {
		if(getAttribute(i).semantic() == KcVertexAttribute::k_diffuse ||
			getAttribute(i).semantic() == KcVertexAttribute::k_specular)
			return true;
	}

	return false;
}


bool KcVertexDeclaration::hasNormal() const
{
	for (unsigned i = 0; i < attributeCount(); i++) {
		if (getAttribute(i).semantic() == KcVertexAttribute::k_normal)
			return true;
	}

	return false;
}


bool KcVertexDeclaration::hasInstance() const
{
	for (unsigned i = 0; i < attributeCount(); i++) {
		if (getAttribute(i).semantic() == KcVertexAttribute::k_instance)
			return true;
	}

	return false;
}


unsigned KcVertexDeclaration::texCoordCount() const
{
	unsigned c(0);
	for(unsigned i = 0; i < attributeCount(); i++) {
		if(getAttribute(i).semantic() == KcVertexAttribute::k_texcoord)
			++c;
	}

	return c;
}


void KcVertexDeclaration::declare() const
{
	auto stride = vertexSize(); // 这个很重要，调试了一晚上才发现。TODO：下一步可优化，避免每次都计算

	for (unsigned i = 0; i < attributeCount(); i++) {
		auto& attr = getAttribute(i);
		glEnableVertexAttribArray(attr.location());
		int type = attr.baseType();
		if (type == KcVertexAttribute::k_float)
			type = GL_FLOAT;
		else if (type == KcVertexAttribute::k_short)
			type = GL_SHORT;
		else if (type == KcVertexAttribute::k_ubyte4)
			type = GL_UNSIGNED_BYTE;
		else {
			assert(false);
		}

		glVertexAttribPointer(attr.location(), attr.componentCount(), type, 
			attr.normalized() ? GL_TRUE : GL_FALSE, stride, ((char*)0) + attr.offset());

		// 支持实例化渲染
		if (attr.semantic() == KcVertexAttribute::k_instance)
			glVertexAttribDivisor(attr.location(), attr.divisor());
	}
}
