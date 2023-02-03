#pragma once
#include "KcRenderObject.h"


// opengl画点的vbo、shader以及uniform参数封装

class KcPointObject : public KcRenderObject
{
	using super_ = KcRenderObject;

public:

	KcPointObject();

	void setSize(float s) {
		pointSize_ = s;
	}

	void draw() const override;

	KcRenderObject* clone() const override;

private:
	float pointSize_{ 2 };
};

