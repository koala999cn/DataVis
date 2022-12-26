#pragma once
#include "KePrimitiveType.h"


class KvGeometry
{
public:

	virtual KePrimitiveType type() const = 0;

	virtual unsigned vertexCount() const = 0;
	virtual void* vertexData() const = 0;

	// ÿ���������ݵ��ֽڳߴ�
	virtual unsigned vertexSize() const = 0;


	virtual unsigned indexCount() const = 0;
	virtual void* indexData() const = 0;

	// ÿ������ֵ���ֽڳߴ�
	virtual unsigned indexSize() const = 0;
};
