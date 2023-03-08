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

	std::pair<unsigned, unsigned> vtxSizePerBar_() const override;

	void drawOneBar_(float_t*, unsigned, float_t, void*, void*, unsigned) const override;
};