#pragma once
#include "KvRenderable.h"
#include "KpContext.h"
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
	KpFont fontText_;
	color4f clrText_{ 0, 0, 0, 1 };

	int barWidth_{ 24 };
	int barLength_{ 0 }; // 0��ʾ��չ��coord����
	int ticks_{ 0 }; // 0��ʾautotick

	KvPlottable* plt_;
	KeAlignment location_; // colorbar��λ��

	bool showBorder_{ true };
	bool showTicker_{ true };
};
