#include "KvPlottable3d.h"
#include "KvDiscreted.h"
#include "KvPaint.h"


void KvPlottable3d::draw(KvPaint* paint) const
{
	auto d = data();

	if (d && d->isDiscreted()) {

		auto disc = std::dynamic_pointer_cast<KvDiscreted>(d);
		if (disc->empty())
			return;

		if (disc->dim() > 1) {
			unsigned ch(0);
			auto getter = [&disc, &ch](unsigned i) -> KvPaint::point3 {
				auto pt = disc->pointAt(i, ch);
				return { pt[0], pt[1], pt[2] };
			};

			for (; ch < disc->channels(); ch++) {
				drawImpl_(paint, getter, majorColor(shareColor() ? 0 : ch));
			}
		}
		else {
			auto defaultZ = defaultZ_;

			unsigned ch(0);
			auto getter = [&disc, &ch, &defaultZ](unsigned i) -> KvPaint::point3 {
				auto pt = disc->pointAt(i, ch);
				return { pt[0], pt[1], defaultZ };
			};

			for (; ch < disc->channels(); ch++) {
				drawImpl_(paint, getter, majorColor(shareColor() ? 0 : ch));
				defaultZ += stepZ_;
			}
		}
	}
}