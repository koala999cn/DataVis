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

	KeAlignment location() const { return align_; }
	KeAlignment& location() { return align_; }

private:

	size_t calcSize_(void*) const override;

	// ����items�ֲ���������
	point2i layouts_() const;

	point2i maxLabelSize_(KvPaint* paint) const;

	void drawItem_(KvPaint* paint, KvPlottable* item, const rect_t& rc) const;

private:

	KpPen border_, iconBorder_;
	KpBrush bkgnd_;
	KpFont fontText_;
	color4f clrText_{ 0, 0, 0, 1 };
	point2i  iconSize_{ 14, 14 }; // { 32, 18 }
	point2i innerMargin_{ 7, 5 }; // { 7, 5, 7, 4 }
	point2i itemSpacing_{ 8, 8 };
	int iconTextPadding_{ 7 };

	bool rowMajor_{ false }; // ��true����item���Ȱ������У��������Ȱ�������
	int maxItemsPerRow_{ 0 }; // ÿ�л�ÿ������item��Ŀ��������ỻ�л��С�<=0����������

	std::vector<KvPlottable*> items_;

	KeAlignment location_; // legend��λ��
};
