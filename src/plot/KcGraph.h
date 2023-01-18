#pragma once
#include "KvPlottable1d.h"
#include "KpContext.h"

// 折线图

class KcGraph : public KvPlottable1d
{
	using super_ = KvPlottable1d;
	using super_::GETTER;

public:

	using super_::super_;

	const color4f& minorColor() const override;

	void setMinorColor(const color4f& minor) override;

	const KpPen& linePen() const { return lineCxt_; }
	KpPen& linePen() { return lineCxt_; }

private:

	void drawImpl_(KvPaint*, GETTER, unsigned, unsigned) const final;

private:
	mutable KpPen lineCxt_;
	color4f clrMinor_{ 0, 0, 0, 1 }; // 辅色，用于绘制渐变线条
};
