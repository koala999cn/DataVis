#pragma once
#include "KvPlottable1d.h"
#include "KpContext.h"


// ɢ��ͼ

class KcScatter : public KvPlottable1d
{
	using super_ = KvPlottable1d;
	using super_::point_getter;

public:

	using super_::super_;

	unsigned majorColorsNeeded() const override;

	bool minorColorNeeded() const override;

	unsigned majorColors() const override;

	color4f majorColor(unsigned idx) const override;

	void setMajorColors(const std::vector<color4f>& majors) override;

	color4f minorColor() const override;

	void setMinorColor(const color4f& minor) override;

	bool showOutline() const { return showOutline_; }
	bool& showOutline() { return showOutline_; }

	const KpMarker& marker() const { return marker_; }
	KpMarker& marker() { return marker_; }

private:

	void drawImpl_(KvPaint*, point_getter, unsigned, unsigned) const override;

protected:
	KpMarker marker_;
	bool showOutline_{ true };
};
