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

	bool showLine() const { return showLine_; }
	bool& showLine() { return showLine_; }

private:

	void drawImpl_(KvPaint*, GETTER, unsigned, unsigned) const final;

	void fillGradiant_(KvPaint*, GETTER, GETTER, unsigned, unsigned) const;

private:
	bool showLine_{ false };
	KpPen lineCxt_;
	mutable KpBrush fillCxt_;
};