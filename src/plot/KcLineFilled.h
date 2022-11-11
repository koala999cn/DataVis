#pragma once
#include "KvPlottable1d.h"


class KcLineFilled : public KvPlottable1d
{
	using super_ = KvPlottable1d;

public:

	using super_::super_;

	unsigned majorColorsNeeded() const override;

	bool minorColorNeeded() const override;

	unsigned majorColors() const override;

	color4f majorColor(unsigned idx) const override;

	void setMajorColors(const std::vector<color4f>& majors) override;

	color4f minorColor() const override;

	void setMinorColor(const color4f& minor) override;

private:

	void drawImpl_(KvPaint*, point_getter, unsigned, unsigned) const final;

private:
	KpPen lineCxt_;
	KpBrush fillCxt_;
};