#pragma once
#include "KePrimitiveType.h"


class KvGeometry
{
public:

	virtual KePrimitiveType type() const = 0;

	virtual unsigned vertexCount() const = 0;
	virtual void* vertexBuffer() const = 0;

	virtual unsigned indexCount() const = 0;
	virtual void* indexBuffer() const = 0;

	// 每个索引值的字节尺寸
	virtual unsigned indexSize() const = 0;
};
