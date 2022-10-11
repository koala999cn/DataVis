#include "KcImPlottable3d.h"
#include "plot/KvPaint.h"
#include "KvDiscreted.h"



void KcImPlottable3d::draw(KvPaint* paint) const
{
	auto d = data();
	if (d && d->isDiscreted()) {

		auto disc = std::dynamic_pointer_cast<KvDiscreted>(d);
		if (disc->empty())
			return;

		auto from = disc->pointAt(0, 0);
		from.resize(3);
		if (disc->dim() == 1)
			from[2] = 0;

		KvPaint::point3 pt0(from[0], from[1], from[2]);

		for (kIndex i = 0; i < disc->size(); i++) {
			auto to = disc->pointAt(i, 0);
			to.resize(3);
			if (disc->dim() == 1)
				to[2] = 0;

			KvPaint::point3 pt1(to[0], to[1], to[2]);
			paint->drawLine(pt0, pt1);

			pt0 = pt1;
		}
	}
}
