#pragma once
#include "KvDecoratorAligned.h"
#include "KpContext.h"
#include <vector>

class KvPlottable;

class KcLegend : public KvDecoratorAligned
{
	using super_ = KvDecoratorAligned;
	using KvRenderable::float_t;
	using point2i = KtPoint<int, 2>;

public:

	KcLegend();

	void draw(KvPaint*) const override;

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

	const color4f& textColor() const { return clrText_; }
	color4f& textColor() { return clrText_; }

	const KpFont& font() const { return font_; }
	KpFont& font() { return font_; }

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

	// 判断plt是否有需要显示的item
	bool hasItem_(KvPlottable* plt) const;

private:
	KpPen iconBorder_;
	margins_t innerMargins_;
	KpFont font_;
	color4f clrText_{ 0, 0, 0, 1 };
	point2f  iconSize_{ 14, 14 }; // { 32, 18 }
	point2f itemSpacing_{ 8, 8 };
	float iconTextPadding_{ 7 };

	bool rowMajor_{ false }; // 若true，则优先按行排列，否则优先按列排列
	int maxItemsPerRow_{ 0 }; // 每行或每列最大的item数目，超过则会换行或换列。<=0代表无限制

	std::vector<KvPlottable*> plts_;
};
