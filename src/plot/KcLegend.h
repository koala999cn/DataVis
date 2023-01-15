#pragma once
#include "KvRenderable.h"
#include "KpContext.h"
#include "layout/KvLayoutElement.h"
#include <vector>

class KvPlottable;

class KcLegend : public KvRenderable, public KvLayoutElement
{
	using super_ = KvRenderable;
	using KvRenderable::float_t;
	using point2i = KtPoint<int, 2>;

public:

	KcLegend();

	void draw(KvPaint*) const override;

	aabb_t boundingBox() const override;

	void addPlottable(KvPlottable* plt) {
		plts_.push_back(plt);
	}

	void removePlottable(KvPlottable* plt) {
		auto pos = std::find(plts_.cbegin(), plts_.cend(), plt);
		plts_.erase(pos);
	}

	unsigned plottableCount() const {
		return plts_.size();
	}

	void clear() {
		plts_.clear();
	}

	const margins_t& innerMargins() const { return innerMargins_; }
	margins_t& innerMargins() { return innerMargins_; }

	KeAlignment location() const { return location_; }
	KeAlignment& location() { return location_; }

	bool showBorder() const { return showBorder_; }
	bool& showBorder() { return showBorder_; }

	const KpPen& borderPen() const { return border_; }
	KpPen& borderPen() { return border_; }

	bool showBkgnd() const { return showBkgnd_; }
	bool& showBkgnd() { return showBkgnd_; }

	const KpBrush& bkgndBrush() const { return bkgnd_; }
	KpBrush& bkgndBrush() { return bkgnd_; }

	const color4f& textColor() const { return clrText_; }
	color4f& textColor() { return clrText_; }

	const point2f& iconSize() const { return iconSize_; }
	point2f& iconSize() { return iconSize_; }

	const point2f& itemSpacing() const { return itemSpacing_; }
	point2f& itemSpacing() { return itemSpacing_; }

	float iconTextPadding() const { return iconTextPadding_; }
	float& iconTextPadding() { return iconTextPadding_; }

	bool rowMajor() const { return rowMajor_; }
	bool& rowMajor() { return rowMajor_; }

	int warps() const { return maxItemsPerRow_; }
	int& warps() { return maxItemsPerRow_; }

	// 计算item数，兼容多通道数据(plt中的每个通道为1个item)
	unsigned itemCount() const;

private:

	size_t calcSize_(void*) const override;

	// 计算items分布的行列数
	point2i layouts_() const;

	point2f maxLabelSize_(KvPaint* paint) const;

	// 绘制plt的第ch个通道item
	void drawItem_(KvPaint* paint, KvPlottable* plt, unsigned ch, const rect_t& rc) const;

	std::string itemLabel_(KvPlottable* plt, unsigned ch) const;

	// 返回>=idx的下一个可见plt
	unsigned nextVisiblePlt_(unsigned idx) const;

	void drawItems_(KvPaint* paint) const;

private:

	bool showBorder_{ true }, showBkgnd_{ false };
	KpPen border_, iconBorder_;
	KpBrush bkgnd_;
	margins_t innerMargins_;
	KpFont fontText_;
	color4f clrText_{ 0, 0, 0, 1 };
	point2f  iconSize_{ 14, 14 }; // { 32, 18 }
	point2f itemSpacing_{ 8, 8 };
	float iconTextPadding_{ 7 };

	bool rowMajor_{ false }; // 若true，则优先按行排列，否则优先按列排列
	int maxItemsPerRow_{ 0 }; // 每行或每列最大的item数目，超过则会换行或换列。<=0代表无限制

	std::vector<KvPlottable*> plts_;

	KeAlignment location_; // legend的位置
};
