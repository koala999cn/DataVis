#pragma once
#include "KcRenderObject.h"


// opengl画点的vbo、shader以及uniform参数封装

class KcPointObject : public KcRenderObject
{
	using super_ = KcRenderObject;

public:

	KcPointObject();

	void setColor(const float4& clr) {
		pointColor_ = clr;
	}

	void setSize(float s) {
		pointSize_ = s;
	}

	void draw() const override;

private:
	float4 pointColor_{ 1, 0, 0, 1 };
	float pointSize_{ 2 };
};

