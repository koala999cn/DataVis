#pragma once
#include "KcVertexAttribute.h"
#include <vector>


class KcVertexDeclaration
{
public:

	void declare() const; // ʹ��opengl��������vertex���Ը�ʽ

	void pushAttribute(const KcVertexAttribute& attr) { 
		attrs_.push_back(attr);
	}

	void pushAttribute(KcVertexAttribute::KeFormat fmt, KcVertexAttribute::KeSemantic semantic, unsigned semanticIndex = 0);

	KcVertexAttribute& getAttribute(unsigned idx) { return attrs_[idx]; }
	const KcVertexAttribute& getAttribute(unsigned idx) const { return attrs_[idx]; }

	const KcVertexAttribute* findAttribute(KcVertexAttribute::KeSemantic semantic, unsigned semanticIdx) const;

	unsigned attributeCount() const { return attrs_.size(); }

	unsigned vertexSize() const; // �������벼����ռ�ֽڵĴ�С

	bool hasSemantic(KcVertexAttribute::KeSemantic semantic) const;

	// ����Ԫ�����Ƿ�����ɫֵ
	bool hasColor() const;

	bool hasNormal() const;

	// ����Ԫ������������Ԫ�ص���Ŀ
	unsigned texCoordCount() const;

private:
	std::vector<KcVertexAttribute> attrs_;
};
