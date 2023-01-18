#pragma once
#include "KvPlottable1d.h"


class KcLineFilled : public KvPlottable1d
{
	using super_ = KvPlottable1d;

public:

	using super_::super_;

	const color4f& minorColor() const override;

	void setMinorColor(const color4f& minor) override;

	const KpBrush& fillBrush() const { return fillCxt_; }
	KpBrush& fillBrush() { return fillCxt_; }

	const KpPen& linePen() const { return lineCxt_; }
	KpPen& linePen() { return lineCxt_; }

private:

	void drawImpl_(KvPaint*, GETTER, unsigned, unsigned) const final;

private:
	KpPen lineCxt_;
	mutable KpBrush fillCxt_;
};