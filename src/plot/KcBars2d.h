#pragma once
#include "KvPlottable.h"
#include "KpContext.h"
#include <functional>


// 2维柱状图.
// 支持堆叠（stacked）、分组（grouped），以及堆叠+分组模式

class KcBars2d : public KvPlottable
{
	using super_ = KvPlottable;

public:

	KcBars2d(const std::string_view& name);

	unsigned majorColorsNeeded() const override;

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

	bool stackedFirst() const { return stackedFirst_; }
	void setStackedFirst(bool b);

	float paddingStacked() const { return paddingStacked_; }
	void setPaddingStacked(float padding);

	float paddingGrouped() const { return paddingGrouped_; }
	void setPaddingGrouped(float padding);


protected:

	unsigned objectCount() const override;

	void setObjectState_(KvPaint*, unsigned objIdx) const override;

	bool objectVisible_(unsigned objIdx) const override;

	void* drawObject_(KvPaint*, unsigned objIdx) const override;

	aabb_t calcBoundingBox_() const override;

	// 计算单个bar的宽度（世界坐标）
	float_t barWidth_(unsigned dim = 0) const;

	struct KpEasyGetter
	{
		// 获取第idx簇的第group分组、第stack堆叠数据
		// NB: 此实质为3个维度的索引
		using GETTER = std::function<std::vector<float_t>(unsigned idx, unsigned group, unsigned stack)>;

		unsigned clusters;
		unsigned groups;
		unsigned stacks;
		GETTER getter;
	};

	KpEasyGetter easyGetter_() const;

	unsigned xdim_() const;

	unsigned ydim_() const;

	bool realFilled_() const;

	bool realEdged_() const;

private:
	mutable KpBrush fill_;
	float barWidthRatio_{ 0.75f }; // barWidth = barWidthRatio_ * dx
	float baseLine_{ 0 }; // bar的底线，各bar的高度 = baseLine_ + y
	bool edged_{ true }, filled_{ true };
	KpPen border_;
	bool stackedFirst_{ false }; // stacked优先还是grouped优先

	// 以下2个间距值均为相对值（为简化起见，均相对于barWidth）
	float paddingStacked_{ 0.f }; // 每个堆叠之内的各bar垂直间隔. 
	float paddingGrouped_{ 0.1f }; // 每个分组之内的各bar水平间距. 
};
