#pragma once
#include "KvRenderable.h"
#include "KpContext.h"
#include <vector>

class KvPlottable;

class KcLegend : public KvRenderable
{
	using super_ = KvRenderable;
	using rect = KtAABB<typename KvRenderable::float_t, 2>;

public:

	KcLegend();

	void draw(KvPaint*) const override;

	aabb_type boundingBox() const override; // TODO: ???

	// 预先计算legend的尺寸（屏幕坐标）
	point2f calcSize(KvPaint*) const;

	void addItem(KvPlottable* plt) {
		items_.push_back(plt);
	}

	void removeItem(KvPlottable* plt) {
		auto pos = std::find(items_.cbegin(), items_.cend(), plt);
		items_.erase(pos);
	}

	void clear() {
		items_.clear();
	}

private:

	// 计算items分布的行列数
	KtPoint<int, 2> layouts_() const;

	KtPoint<int, 2> maxLabelSize_(KvPaint* paint) const;

	void drawItem_(KvPaint* paint, KvPlottable* item, const rect& rc) const;

private:

	KpPen border_, iconBorder_;
	KpBrush bkgnd_;
	KpFont fontText_;
	color4f clrText_{ 0, 0, 0, 1 };
	KtPoint<int, 2>  iconSize_{ 16, 16 }; // { 32, 18 }
	KtPoint<int, 2> innerMargin_{ 7, 5 }; // { 7, 5, 7, 4 }
	KtPoint<int, 2> outterMargin_{ 7, 5 };
	KtPoint<int, 2> itemSpacing_{ 8, 8 };
	int iconTextPadding_{ 7 };

	bool rowMajor_{ false }; // 若true，则item优先按行排列，否则优先按列排列
	int maxItemsPerRow_{ 0 }; // 每行或每列最大的item数目，超过则会换行或换列。<=0代表无限制

	std::vector<KvPlottable*> items_;
};
