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

	// Ԥ�ȼ���legend�ĳߴ磨��Ļ���꣩
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

	// KeAlignment�����
	int alignment() const { return align_; }
	void setAlignment(int align) { align_ = align; }

private:

	// ����items�ֲ���������
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

	bool rowMajor_{ false }; // ��true����item���Ȱ������У��������Ȱ�������
	int maxItemsPerRow_{ 0 }; // ÿ�л�ÿ������item��Ŀ��������ỻ�л��С�<=0����������

	std::vector<KvPlottable*> items_;

	int align_;
};
