#pragma once
#include "KvPlottable1d.h"
#include "KpContext.h"
#include <functional>


// 2维柱状图.
// 支持堆叠（stacked）、分组（grouped），以及堆叠+分组模式

class KcBars2d : public KvPlottable1d
{
	using super_ = KvPlottable1d;

public:

	KcBars2d(const std::string_view& name);

	const color4f& minorColor() const override;

	void setMinorColor_(const color4f& minor) override;

	bool showFill() const { return filled_; }
	bool& showFill() { return filled_; }

	bool showBorder() const { return edged_; }
	bool& showBorder() { return edged_; }

	// bar的宽度设置，取值[0, 1]
	// 若取1，则bars之间无空隙；若取0.5，则bar的宽度与bars之间的空隙相同。
	float barWidthRatio() const { return barWidthRatio_; }
	void setBarWidthRatio(float w);

	float baseLine() const { return baseLine_; }
	void setBaseLine(float base);

	const KpBrush& fillBrush() const { return fill_; }
	KpBrush& fillBrush() { return fill_; }

	const KpPen& borderPen() const { return border_; }
	KpPen& borderPen() { return border_; }

	float paddingStacked() const { return paddingStacked_; }
	void setPaddingStacked(float padding);


protected:

	unsigned objectCount() const override;

	void setObjectState_(KvPaint*, unsigned objIdx) const override;

	bool objectVisible_(unsigned objIdx) const override;

	void* drawObject_(KvPaint*, unsigned objIdx) const override;

	aabb_t calcBoundingBox_() const override;

	// 计算单个bar的宽度（世界坐标）
	float_t barWidth_(unsigned dim) const;

	bool realFilled_() const;

	bool realEdged_() const;

private:
	mutable KpBrush fill_;
	float barWidthRatio_{ 0.75f }; // barWidth = barWidthRatio_ * dx
	float baseLine_{ 0 }; // bar的底线，各bar的高度 = baseLine_ + y
	bool edged_{ true }, filled_{ true };
	KpPen border_;

	float paddingStacked_{ 0.f }; // 每个堆叠之内的各bar垂直间隔（像素值）
};
