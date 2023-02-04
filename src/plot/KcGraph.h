#pragma once
#include "KvPlottable1d.h"
#include "KpContext.h"

// 折线图

class KcGraph : public KvPlottable1d
{
	using super_ = KvPlottable1d;

public:

	using super_::super_;

	const color4f& minorColor() const override;

	void setMinorColor_(const color4f& minor) override;

	const KpPen& linePen() const { return lineCxt_; }
	KpPen& linePen() { return lineCxt_; }

protected:

	void drawImpl_(KvPaint*, GETTER, unsigned, unsigned) const final;

private:
	mutable KpPen lineCxt_;
	color4f clrMinor_{ 0, 0, 0, 1 }; // 辅色，用于绘制渐变线条
	mutable std::vector<void*> renderObj_;
};
