#include "KcVertexDeclaration.h"
#include "glad.h"
#include <assert.h>


const KcVertexAttribute* KcVertexDeclaration::findAttribute(KcVertexAttribute::KeSemantic semantic, unsigned semanticIdx) const
{
	for(unsigned i = 0; i < attributeCount(); i++) {
		const auto& attr = getAttribute(i);
		if(attr.semantic() == semantic && attr.semanticIndex() == semanticIdx)
			return &attr;
	}

	return nullptr;
}


unsigned KcVertexDeclaration::calcVertexSize() const
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
	for (unsigned i = 0; i < attributeCount(); i++) {
		auto& attr = getAttribute(i);
		glEnableVertexAttribArray(attr.location());
		int type = attr.baseType();
		if (type == KcVertexAttribute::k_float)
			type = GL_FLOAT;
		else if (type == KcVertexAttribute::k_short)
			type = GL_SHORT;
		else {
			assert(false);
		}

		glVertexAttribPointer(attr.location(), attr.componentCount(), type, 
			attr.normalized() ? GL_TRUE : GL_FALSE, 0, (void*)attr.offset()); // TODO: stride
	}
}
