#pragma once
#include "KcBars2d.h"
#include "KpContext.h"


// 3Î¬Öù×´Í¼

class KcBars3d : public KcBars2d
{
	using super_ = KcBars2d;

public:

	using super_::super_;

private:

	aabb_t calcBoundingBox_() const override;

	//void drawImpl_(KvPaint*, point_getter1, unsigned, unsigned) const override;
};