#pragma once
#include "KcBars2d.h"
#include "KpContext.h"


// 3Î¬Öù×´Í¼

class KcBars3d : public KcBars2d
{
	using super_ = KcBars2d;

public:

	using super_::super_;

	float barWidthRatioZ() const { return barWidthRatioZ_; }
	void setBarWidthRatioZ(float w);

private:

	aabb_t calcBoundingBox_() const override;

	std::pair<unsigned, unsigned> vtxSizePerBar_() const override;

	void drawOneBar_(float_t*, unsigned, float_t, void*, void*, unsigned) const override;

private:
	float barWidthRatioZ_{ 0.75f }; 
};