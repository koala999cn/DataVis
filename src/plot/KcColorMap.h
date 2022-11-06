#pragma once
#include "KvPlottable.h"
#include "KtGradient.h"


class KcColorMap : public KvPlottable
{
	using super_ = KvPlottable;

public:

	using super_::super_;

	unsigned majorColorsNeeded() const override { return -1; }

	bool minorColorNeeded() const override { return true; }

	void draw(KvPaint*) const override;

protected:

	color4f mapValueToColor_(float_t val) const;

private:
	float_t mapLower_{ 0 }, mapUpper_{ 1 }; // color mapµÄÖµÓò·¶Î§

	KtGradient<float_t, color4f> mapper_;
};
