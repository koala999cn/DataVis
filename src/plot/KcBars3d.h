#pragma once
#include "KcBars2d.h"
#include "KpContext.h"


// 3Î¬Öù×´Í¼

class KcBars3d : public KcBars2d
{
	using super_ = KcBars2d;
	using KvPlottable1d::point_getter;

public:

	using super_::super_;

	aabb_type boundingBox() const override;

private:

	void drawImpl_(KvPaint*, point_getter, unsigned, unsigned) const override;
};