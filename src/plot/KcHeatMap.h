#pragma once
#include "KvPlottable2d.h"

// »»Õº

class KcHeatMap : public KvPlottable2d
{
	using super_ = KvPlottable2d;

public:

	using super_::super_;

	aabb_t boundingBox() const override;

	bool showText() const { return showText_; }
	bool& showText() { return showText_; }

	color4f textColor() const { return clrText_; }
	color4f& textColor() { return clrText_; }

private:
	void drawImpl_(KvPaint*, point_getter2, unsigned nx, unsigned ny, unsigned channels) const final;

private:

	bool showText_{ false };
	color4f clrText_{ 1, 0, 0, 1 };
};
