#include "KcBars2d.h"
#include "KvPaint.h"


void KcBars2d::drawImpl_(KvPaint* paint, point_getter getter, const color4f& majorColor) const
{
	for (unsigned i = 0; i < data()->size(); i++) {
		auto pt0 = getter(i);
		pt0.x() -= width_ * 0.5;
		point3 pt1(pt0.x() + width_, 0, pt0.z());

		if (fill_.style != KpBrush::k_none) {
			auto cxt = fill_;
			cxt.color = majorColor;
			paint->apply(cxt);
			paint->fillRect(pt0, pt1);
		}

		if (border_.style != KpPen::k_none) {
			auto cxt = border_;
			cxt.color = minorColor();
			paint->apply(cxt);
			paint->drawRect(pt0, pt1);
		}
	}
}