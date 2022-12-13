#include "KcVertexDeclaration.h"


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