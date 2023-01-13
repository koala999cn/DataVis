#pragma once
#include "KvPlottable1d.h"
#include "KpContext.h"


// 2维柱状图

class KcBars2d : public KvPlottable1d
{
	using super_ = KvPlottable1d;
	using super_::point_getter1;

public:

	KcBars2d(const std::string_view& name);

	aabb_t boundingBox() const override;

	const color4f& minorColor() const override;

	void setMinorColor(const color4f& minor) override;

	// bar的宽度设置，取值[0, 1]
	// 若取1，则bars之间无空隙；若取0.5，则bar的宽度与bars之间的空隙相同。
	float barWidthRatio() const { return barWidthRatio_; }
	float& barWidthRatio() { return barWidthRatio_; }

	float baseLine() const { return baseLine_; }
	float& baseLine() { return baseLine_; }

	const KpBrush& fillBrush() const { return fill_; }
	KpBrush& fillBrush() { return fill_; }

	const KpPen& borderPen() const { return border_; }
	KpPen& borderPen() { return border_; }

protected:

	void drawImpl_(KvPaint*, point_getter1, unsigned, unsigned) const override;

	// 计算单个bar的宽度（世界坐标）
	float_t barWidth_(unsigned dim = 0) const;

protected:
	KpPen border_;
	mutable KpBrush fill_;
	float barWidthRatio_{ 0.5f };
	float baseLine_{ 0 }; // bar的底线
};
