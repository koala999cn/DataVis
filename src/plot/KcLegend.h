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

	// Ԥ�ȼ���legend�ĳߴ磨��Ļ���꣩
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

	// ����items�ֲ���������
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

	bool rowMajor_{ false }; // ��true����item���Ȱ������У��������Ȱ�������
	int maxItemsPerRow_{ 0 }; // ÿ�л�ÿ������item��Ŀ��������ỻ�л��С�<=0����������

	std::vector<KvPlottable*> items_;
};
