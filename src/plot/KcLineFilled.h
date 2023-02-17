#pragma once
#include "KvPlottable1d.h"


class KcLineFilled : public KvPlottable1d
{
	using super_ = KvPlottable1d;

public:

	using super_::super_;

	const color4f& minorColor() const override;

	void setMinorColor_(const color4f& minor) override;

	const KpBrush& fillBrush() const { return fillCxt_; }
	KpBrush& fillBrush() { return fillCxt_; }

	const KpPen& linePen() const { return lineCxt_; }
	KpPen& linePen() { return lineCxt_; }

	bool showLine() const { return showLine_; }
	bool& showLine() { return showLine_; }

private:

	unsigned objectsPerBatch_() const final { return 2; } // fill和edge分别有1个渲染对象

	bool objectVisible_(unsigned objIdx) const override;

	void setObjectState_(KvPaint*, unsigned objIdx) const final;

	void* drawObjectImpl_(KvPaint*, GETTER, unsigned count, unsigned objIdx) const final;

	void* fillGradiant_(KvPaint*, GETTER, GETTER, unsigned, unsigned) const;

private:
	bool showLine_{ false };
	KpPen lineCxt_;
	mutable KpBrush fillCxt_;
};