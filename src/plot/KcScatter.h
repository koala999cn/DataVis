#pragma once
#include "KvPlottable1d.h"
#include "KpContext.h"


// …¢µ„Õº

class KcScatter : public KvPlottable1d
{
	using super_ = KvPlottable1d;
	using super_::point_getter;

public:

	using super_::super_;

	void drawImpl_(KvPaint*, point_getter, unsigned, const color4f&) const override;

protected:
	KpPen scatPen_;
	KpBrush scatBrush_;
	float size_{ 3 };
};
