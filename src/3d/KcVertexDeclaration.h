#pragma once
#include "KcVertexAttribute.h"
#include <vector>


class KcVertexDeclaration
{
public:

	void declare(bool enable = true) const; // 使用opengl函数声明vertex属性格式

	void pushAttribute(const KcVertexAttribute& attr) { 
		attrs_.push_back(attr);
	}

	void pushAttribute(KcVertexAttribute::KeFormat fmt, KcVertexAttribute::KeSemantic semantic, unsigned semanticIndex = 0);

	KcVertexAttribute& getAttribute(unsigned idx) { return attrs_[idx]; }
	const KcVertexAttribute& getAttribute(unsigned idx) const { return attrs_[idx]; }

	const KcVertexAttribute* findAttribute(KcVertexAttribute::KeSemantic semantic, unsigned semanticIdx) const;

	unsigned attributeCount() const { return attrs_.size(); }

	unsigned vertexSize() const; // 计算输入布局所占字节的大小

	bool hasSemantic(int semantic) const;

	// 顶点元素中是否含有颜色值
	bool hasColor() const;

	bool hasNormal() const;

	bool hasInstance() const;

	// 顶点元素中纹理坐标元素的数目
	unsigned texCoordCount() const;

private:
	std::vector<KcVertexAttribute> attrs_;
};

