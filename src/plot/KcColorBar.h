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

	void resetPlottable(KvPlottable* plt);

	void draw(KvPaint*) const override;

	aabb_t boundingBox() const override;

	KeAlignment location() const { return location_; }
	KeAlignment& location() { return location_; }

	bool showBorder() const { return showBorder_; }
	bool& showBorder() { return showBorder_; }

	const KpPen& borderPen() const { return border_; }
	KpPen& borderPen() { return border_; }

	float barWidth() const { return barWidth_; }
	float& barWidth() { return barWidth_; }

	float barLength() const { return barLength_; }
	float& barLength() { return barLength_; }

	const KcAxis& axis() const { return *axis_; }
	KcAxis& axis() { return *axis_; }

private:

	size_t calcSize_(void*) const final;

private:

	bool showBorder_{ true };
	KpPen border_;

	float barWidth_{ 24 }; // 像素值
	float barLength_{ 0 }; // 像素值. 0表示延展与coord-plane对齐
	int ticks_{ 0 }; // 0表示autotick

	KvPlottable* plt_;
	KeAlignment location_; // colorbar的位置
	
	std::unique_ptr<KcAxis> axis_; // 用于绘制tick和label
};
