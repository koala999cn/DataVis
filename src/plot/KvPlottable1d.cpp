#include "KvPlottable1d.h"
#include "KvDiscreted.h"
#include "KvContinued.h"
#include "KvPaint.h"
#include "KtSampling.h"


void KvPlottable1d::draw(KvPaint* paint) const
{
	auto d = data();
	if (d == nullptr || d->size() == 0 || d->length(0) == 0)
		return;

	if (d->isDiscreted()) {

		auto disc = std::dynamic_pointer_cast<KvDiscreted>(d);

		if (disc->dim() > 1) {
			unsigned ch(0);
			auto getter = [&disc, &ch](unsigned i) -> KvPaint::point3 {
				auto pt = disc->pointAt(i, ch);
				return { pt[0], pt[1], pt[2] };
			};

			for (; ch < disc->channels(); ch++) {
				drawImpl_(paint, getter, disc->size(), majorColor(shareColor() ? 0 : ch));
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
				drawImpl_(paint, getter, disc->size(), majorColor(shareColor() ? 0 : ch));
				defaultZ += stepZ_;
			}
		}
	}
	else if (d->isContinued()) {

		auto cont = std::dynamic_pointer_cast<KvContinued>(d);
		KtSampling<kReal> samp;
		samp.resetn(1000, cont->range(0).low(), cont->range(0).high(), 0); // TODO: 暂时固定显示1000个点

		unsigned ch(0);
		auto defaultZ = defaultZ_;
		auto getter = [&cont, &samp, &ch, &defaultZ](unsigned i) -> KvPaint::point3 {
			kReal x = samp.indexToX(i);
			kReal y = cont->value(x, ch);
			return { x, y, defaultZ };
		};

		for (; ch < cont->channels(); ch++) {
			drawImpl_(paint, getter, samp.size(), majorColor(shareColor() ? 0 : ch));
			defaultZ += stepZ_;
		}
	}
}