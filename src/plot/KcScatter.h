#pragma once
#include "KvPlottable1d.h"
#include "KpContext.h"


// …¢µ„Õº

class KcScatter : public KvPlottable1d
{
	using super_ = KvPlottable1d;

public:

	using super_::super_;

	const color4f& minorColor() const override;

	void setMinorColor_(const color4f& minor) override;

	const KpMarker& marker() const { return marker_; }
	KpMarker& marker() { return marker_; }

	const KpPen& linePen() const { return lineCxt_; }
	KpPen& linePen() { return lineCxt_; }

	bool showLine() const { return showLine_; }
	bool& showLine() { return showLine_; }

private:

	void drawImpl_(KvPaint*, GETTER, unsigned, unsigned) const override;

protected:
	bool showLine_{ false };
	KpPen lineCxt_;
	mutable KpMarker marker_;
};
