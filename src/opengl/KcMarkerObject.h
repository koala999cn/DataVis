#pragma once
#include "KcRenderObject.h"


class KcMarkerObject : public KcRenderObject
{
	using super_ = KcRenderObject;

public:
	using super_::super_;

	void draw() const override;

	KcRenderObject* clone() const override;

	void setSize(float sz) { size_ = sz; }

private:
	float size_{ 1 };
};
