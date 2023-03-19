#pragma once
#include "KcRenderObject.h"


class KcMarkerObject : public KcRenderObject
{
	using super_ = KcRenderObject;

public:
	using super_::super_;

	void draw() const override;

	KcRenderObject* clone() const override;

	void setScale(point2f sz) { size_ = sz; }

private:
	point2f size_{ 1 };
};
