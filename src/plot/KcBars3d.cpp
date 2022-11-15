#include "KcBars3d.h"
#include "KuGeometryFactory.h"
#include "KvData.h"


KcBars3d::aabb_type KcBars3d::boundingBox() const
{
	auto aabb = super_::boundingBox();

	// TODO: 
	return aabb;
}


void KcBars3d::drawImpl_(KvPaint* paint, point_getter getter, unsigned count, unsigned) const
{
	auto dim = data()->dim();
	auto xw = barWidth_(0);
	auto yw = barWidth_(dim == 1 ? 2 : 1);

	bool drawFill = fill_.style != KpBrush::k_none && majorColor(0).a() != 0;
	bool drawBorder = border_.style != KpPen::k_none && minorColor().a() != 0 && minorColor() != majorColor(0);

	for (unsigned i = 0; i < count; i++) {
		auto pt0 = getter(i);
		point3 pt1;

		if (dim == 1) {
			pt0.x() += xw * 0.5; pt0.z() += yw * 0.5;
			pt1 = { pt0.x() - xw, baseLine_, pt0.z() - yw };
		}
		else {
			pt0.x() += xw * 0.5; pt0.y() += yw * 0.5;
			pt1 = { pt0.x() - xw, pt0.y() - yw, baseLine_ };
		}

		auto box = KuGeometryFactory::makeBox(pt1, pt0);

		if (drawFill) {
			paint->apply(fill_);
			paint->drawGeom(box);
		}

		// TODO:
		//if (drawBorder) {
		//	paint->apply(border_);
		//	paint->drawRect(pt0, pt1);
		//}
	}
}