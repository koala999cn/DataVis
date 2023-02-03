#pragma once
#include "KcRenderObject.h"


// 带全局光照（灯光方向+Z）的渲染对象

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

