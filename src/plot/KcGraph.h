#pragma once
#include "KvPlottable1d.h"
#include "KpContext.h"

// ’€œﬂÕº

class KcGraph : public KvPlottable1d
{
	using super_ = KvPlottable1d;
	using super_::point_getter1;

public:

	using super_::super_;

	const color4f& minorColor() const override;

	void setMinorColor(const color4f& minor) override;

	const KpPen& linePen() const { return lineCxt_; }
	KpPen& linePen() { return lineCxt_; }

private:

	void drawImpl_(KvPaint*, point_getter1, unsigned, unsigned) const final;

private:
	mutable KpPen lineCxt_;
};
