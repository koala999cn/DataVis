#pragma once
#include "KvRenderable.h"
#include "KpContext.h"
#include <vector>

class KvPlottable;

class KcLegend : public KvRenderable
{
	using super_ = KvRenderable;
	using rect = KtAABB<float_t, 2>;
	using point2i = KtPoint<int, 2>;

public:

	KcLegend();

	void draw(KvPaint*) const override;

	aabb_t boundingBox() const override;

	// 预先计算legend的尺寸（屏幕坐标）
	point2i calcSize(KvPaint*) const;

	void addItem(KvPlottable* plt) {
		items_.push_back(plt);
	}

	void removeItem(KvPlottable* plt) {
		auto pos = std::find(items_.cbegin(), items_.cend(), plt);
		items_.erase(pos);
	}

	unsigned itemCount() const {
		return items_.size();
	}

	void clear() {
		items_.clear();
	}

	// KeAlignment的组合
	int alignment() const { return align_; }
	void setAlignment(int align) { align_ = align; }

private:

	// 计算items分布的行列数
	point2i layouts_() const;

	point2i maxLabelSize_(KvPaint* paint) const;

	void drawItem_(KvPaint* paint, KvPlottable* item, const rect& rc) const;

private:

	KpPen border_, iconBorder_;
	KpBrush bkgnd_;
	KpFont fontText_;
	color4f clrText_{ 0, 0, 0, 1 };
	point2i  iconSize_{ 14, 14 }; // { 32, 18 }
	point2i innerMargin_{ 7, 5 }; // { 7, 5, 7, 4 }
	point2i outterMargin_{ 7, 5 };
	point2i itemSpacing_{ 8, 8 };
	int iconTextPadding_{ 7 };

	bool rowMajor_{ false }; // 若true，则item优先按行排列，否则优先按列排列
	int maxItemsPerRow_{ 0 }; // 每行或每列最大的item数目，超过则会换行或换列。<=0代表无限制

	std::vector<KvPlottable*> items_;

	int align_;
};
