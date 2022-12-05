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

	int barWidth_{ 24 }; // ����ֵ
	int barLength_{ 0 }; // ����ֵ. 0��ʾ��չ��coord-plane����
	int ticks_{ 0 }; // 0��ʾautotick

	KvPlottable* plt_;
	KeAlignment location_; // colorbar��λ��

	bool showBorder_{ true };
	
	std::unique_ptr<KcAxis> axis_; // ���ڻ���tick��label
};
