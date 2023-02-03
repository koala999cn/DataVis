#pragma once
#include "KcRenderObject.h"


// opengl�����vbo��shader�Լ�uniform������װ

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

