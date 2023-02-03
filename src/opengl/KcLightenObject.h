#pragma once
#include "KcRenderObject.h"


// ��ȫ�ֹ��գ��ƹⷽ��+Z������Ⱦ����

class KcLightenObject : public KcRenderObject
{
	using super_ = KcRenderObject;

public:

	KcLightenObject(KePrimitiveType type);

	void setNormalMatrix(float4x4<> m) {
		normalMat_ = m;
	}

	void draw() const override;

	KcRenderObject* clone() const override;

private:
	float4x4<> normalMat_;
};

