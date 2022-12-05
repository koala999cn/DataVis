#pragma once
#include "KvRenderable.h"
#include "KpContext.h"
#include "KcAxis.h"
#include "layout/KvLayoutElement.h"

class KvPlottable;

class KcColorBar : public KvRenderable, public KvLayoutElement
{
public:

	KcColorBar(KvPlottable* plt);

	void draw(KvPaint*) const override;

	aabb_t boundingBox() const override;

	KeAlignment location() const { return location_; }
	KeAlignment& location() { return location_; }

private:

	size_t calcSize_(void*) const final;

private:

	KpPen border_;

	int barWidth_{ 24 }; // 像素值
	int barLength_{ 0 }; // 像素值. 0表示延展与coord-plane对齐
	int ticks_{ 0 }; // 0表示autotick

	KvPlottable* plt_;
	KeAlignment location_; // colorbar的位置

	bool showBorder_{ true };
	
	std::unique_ptr<KcAxis> axis_; // 用于绘制tick和label
};
