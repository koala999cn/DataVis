#pragma once
#include "KcRenderObject.h"


// opengl画线的vbo、shader以及uniform参数封装

class KcLineObject : public KcRenderObject
{
	using super_ = KcRenderObject;

public:

	KcLineObject(KeType type = k_line_strip);

	void setColor(const float4& clr) {
		lineColor_ = clr;
	}

	void setWidth(float w) {
		lineWidth_ = w;
	}

	void draw() const override;

private:
	float4 lineColor_{1, 0, 0, 1};
	float lineWidth_{ 1 };
};