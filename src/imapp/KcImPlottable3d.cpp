#include "KcImPlottable3d.h"
#include "plot/KvPaint3d.h"
#include "KvDiscreted.h"



void KcImPlottable3d::draw(KvPaint3d* paint) const
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

		auto getter = [&disc](unsigned i) -> KvPaint3d::point3 {
			auto pt = disc->pointAt(i, 0);
			pt.resize(3);
			if (disc->dim() == 1)
				pt[2] = 0;

			return { pt[0], pt[1], pt[2] };
		};

		paint->drawLineStrip(getter, disc->size());
	}
}
