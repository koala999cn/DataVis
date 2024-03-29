#pragma once
#include "KcRenderObject.h"


// opengl画线的vbo、shader以及uniform参数封装

class KcLineObject : public KcRenderObject
{
	using super_ = KcRenderObject;

public:

	KcLineObject(KePrimitiveType type = k_line_strip);

	void setWidth(float w) {
		lineWidth_ = w;
	}

	void setStyle(int style) {
		lineStyle_ = style;
	}

	void draw() const override;

	KcRenderObject* clone() const override;

private:
	float lineWidth_{ 1 };
	int lineStyle_{ 0 };
};