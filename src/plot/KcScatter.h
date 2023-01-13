#pragma once
#include "KvPlottable1d.h"
#include "KpContext.h"


// ɢ��ͼ

class KcScatter : public KvPlottable1d
{
	using super_ = KvPlottable1d;
	using super_::point_getter1;

public:

	using super_::super_;

	const color4f& minorColor() const override;

	void setMinorColor(const color4f& minor) override;

	const KpMarker& marker() const { return marker_; }
	KpMarker& marker() { return marker_; }

private:

	void drawImpl_(KvPaint*, point_getter1, unsigned, unsigned) const override;

protected:
	mutable KpMarker marker_;
};
