#pragma once
#include "KePrimitiveType.h"


class KvGeometry
{
public:

	virtual KePrimitiveType type() const = 0;

	virtual unsigned vertexCount() const = 0;
	virtual void* vertexData() const = 0;

	// 每个顶点数据的字节尺寸
	virtual unsigned vertexSize() const = 0;


	virtual unsigned indexCount() const = 0;
	virtual void* indexData() const = 0;

	// 每个索引值的字节尺寸
	virtual unsigned indexSize() const = 0;
};
