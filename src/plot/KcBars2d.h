#pragma once
#include "KvPlottable1d.h"
#include "KpContext.h"


// 2Î¬Öù×´Í¼

class KcBars2d : public KvPlottable1d
{
	using super_ = KvPlottable1d;
	using super_::point_getter;

public:

	using super_::super_;

	void drawImpl_(KvPaint*, point_getter, const color4f&) const override;

protected:
	KpPen border_;
	KpBrush fill_;
	float width_{ 6 };
};
