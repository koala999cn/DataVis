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

		unsigned ch(0);
		auto getter = [&disc, &ch](unsigned i) -> KvPaint3d::point3 {
			auto pt = disc->pointAt(i, ch);
			pt.resize(3);
			if (disc->dim() == 1)
				pt[2] = 0;

			return { pt[0], pt[1], pt[2] };
		};

		for(; ch < disc->channels(); ch++)
		    paint->drawPoints(getter, disc->size());
	}
}
