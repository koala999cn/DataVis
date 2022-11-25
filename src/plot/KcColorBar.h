#pragma once
#include "KvRenderable.h"
#include "KpContext.h"

class KvPlottable;

class KcColorBar : public KvRenderable
{
	using super_ = KvRenderable;
	using rect = KtAABB<float_t, 2>;
	using point2i = KtPoint<int, 2>;

public:

	KcColorBar(KvPlottable* plt);

	void draw(KvPaint*) const override;

	aabb_t boundingBox() const override;

	// 预先计算color-bar的尺寸（屏幕坐标）
	point2i calcSize(KvPaint*) const;

	// KeAlignment的组合
	int alignment() const { return align_; }
	void setAlignment(int align) { align_ = align; }

private:

	KpPen border_;
	KpBrush bkgnd_;
	KpFont fontText_;
	color4f clrText_{ 0, 0, 0, 1 };

	int barWidth_{ 24 };
	int barLength_{ 0 }; // 0表示延展与coord对齐

	KvPlottable* plt_;
	int align_;
};
