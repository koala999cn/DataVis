#pragma once
#include "KcVertexAttribute.h"
#include <vector>


class KcVertexDeclaration
{
public:

	void declare() const; // 使用opengl函数声明vertex属性格式

	void pushAttribute(const KcVertexAttribute& attr) { 
		attrs_.push_back(attr);
	}

	KcVertexAttribute& getAttribute(unsigned idx) { return attrs_[idx]; }
	const KcVertexAttribute& getAttribute(unsigned idx) const { return attrs_[idx]; }

	const KcVertexAttribute* findAttribute(KcVertexAttribute::KeSemantic semantic, unsigned semanticIdx) const;

	unsigned attributeCount() const { return attrs_.size(); }

	unsigned calcVertexSize() const; // 计算输入布局所占字节的大小

	bool hasSemantic(KcVertexAttribute::KeSemantic semantic) const;

	// 顶点元素中是否含有颜色值
	bool hasColor() const;

	// 顶点元素中纹理坐标元素的数目
	unsigned texCoordCount() const;

private:
	std::vector<KcVertexAttribute> attrs_;
};

