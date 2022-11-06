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

	unsigned majorColorsNeeded() const override;

	bool minorColorNeeded() const override;

	unsigned majorColors() const override;

	const color4f& majorColor(unsigned idx) const override;
	color4f& majorColor(unsigned idx) override;

	void setMajorColors(const std::vector<color4f>& majors) override;

	const color4f& minorColor() const override;
	color4f& minorColor() override;

	void setMinorColor(const color4f& minor) override;

private:

	void drawImpl_(KvPaint*, point_getter, unsigned, unsigned) const override;

protected:
	KpPen scatPen_;
	KpBrush scatBrush_;
	float size_{ 3 };
};
