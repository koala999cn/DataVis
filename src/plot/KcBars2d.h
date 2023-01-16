#pragma once
#include "KvPlottable.h"
#include "KpContext.h"


// 2维柱状图

class KcBars2d : public KvPlottable
{
	using super_ = KvPlottable;

public:

	KcBars2d(const std::string_view& name);

	aabb_t boundingBox() const override;

	const color4f& minorColor() const override;

	void setMinorColor(const color4f& minor) override;

	bool showBorder() const { return showBorder_; }
	bool& showBorder() { return showBorder_; }

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

	bool stacked() const { return stacked_; }
	bool& stacked() { return stacked_; }

protected:

	void drawDiscreted_(KvPaint*, KvDiscreted*) const override;

	// 计算单个bar的宽度（世界坐标）
	float_t barWidth_(unsigned dim = 0) const;

protected:
	KpPen border_;
	mutable KpBrush fill_;
	float barWidthRatio_{ 0.5f };
	float baseLine_{ 0 }; // bar的底线
	bool showBorder_{ true };
	bool stacked_{ true }; // stacked-bar or grouped-bar
	float paddingRatio_{ 0.1 }; // 每组bar之间的间隔. 相对于bar宽度，即 padding = paddingRatio_ * barWidth
};
