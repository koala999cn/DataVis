#pragma once
#include "KvPlottable1d.h"
#include "KpContext.h"


// …¢µ„Õº

class KcScatter : public KvPlottable1d
{
	using super_ = KvPlottable1d;
	using super_::point_getter1;

public:

	using super_::super_;

	unsigned majorColorsNeeded() const override;

	bool minorColorNeeded() const override;

	unsigned majorColors() const override;

	color4f majorColor(unsigned idx) const override;

	void setMajorColors(const std::vector<color4f>& majors) override;

	color4f minorColor() const override;

	void setMinorColor(const color4f& minor) override;

	const KpMarker& marker() const { return marker_; }
	KpMarker& marker() { return marker_; }

private:

	void drawImpl_(KvPaint*, point_getter1, unsigned, unsigned) const override;

protected:
	KpMarker marker_;
};
