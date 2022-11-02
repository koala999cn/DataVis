#include "KvPlottable1d.h"
#include "KvDiscreted.h"
#include "KvContinued.h"
#include "KvPaint.h"
#include "KtSampling.h"
#include "KtSampler.h"


void KvPlottable1d::draw(KvPaint* paint) const
{
	auto d = data();
	if (d == nullptr || d->size() == 0)
		return;

	for (unsigned i = 0; i < d->dim(); i++)
		if (d->length(i) == 0)
			return;

	if (d->dim() == 1) {
		draw1d_(paint);
	}
	else if (d->isDiscreted()) {
		auto disc = std::dynamic_pointer_cast<KvDiscreted>(d);
		if (disc->isSampled())
			draw2d_(paint);
		else
			draw3d_(paint);

	}
	else { //if (d->isContinued()) 
		draw2d_(paint);
	}
}


void KvPlottable1d::draw1d_(KvPaint* paint) const
{
	auto d = data();
	assert(d && d->dim() == 1);

	auto disc = std::dynamic_pointer_cast<KvDiscreted>(d);
	if (disc == nullptr) {
		auto cont = std::dynamic_pointer_cast<KvContinued>(d);
		assert(cont);

		auto samp = std::make_shared<KtSampler<1>>(cont);
		samp->reset(0, sampCount(0), cont->range(0).low(), cont->range(0).high(), 0);
		disc = samp;
	}

	auto defaultZ = defaultZ_;

	unsigned ch(0);
	auto getter = [&disc, &ch, &defaultZ](unsigned i) -> KvPaint::point3 {
		auto pt = disc->pointAt(i, ch);
		return { pt[0], pt[1], defaultZ };
	};

	for (; ch < disc->channels(); ch++) {
		drawImpl_(paint, getter, disc->size(0), majorColor(shareColor() ? 0 : ch));
		defaultZ += stepZ_;
	}
}


void KvPlottable1d::draw2d_(KvPaint* paint) const
{
	auto d = data();
	assert(d && d->dim() == 2);

	auto disc = std::dynamic_pointer_cast<KvDiscreted>(d);
	if (disc == nullptr) {
		auto cont = std::dynamic_pointer_cast<KvContinued>(d);
		assert(cont);

		auto samp = std::make_shared<KtSampler<2>>(cont);
		samp->reset(0, sampCount(0), cont->range(0).low(), cont->range(0).high(), 0);
		samp->reset(1, sampCount(1), cont->range(1).low(), cont->range(1).high(), 0);
		disc = samp;
	}

	assert(disc->isSampled());

	// TODO: 暂时只处理第1个通道
	kIndex idx[2];
	auto getter = [&disc, &idx](unsigned i) -> KvPaint::point3 {
		idx[1] = i;
		auto n = idx[0] * disc->size(1) * disc->channels() + i;
		auto pt = disc->pointAt(n, 0);
		return { pt[0], pt[1], pt[2] };
	};

	for (kIndex ix = 0; ix < disc->size(0); ix++) {
		idx[0] = ix;
		drawImpl_(paint, getter, disc->size(1), majorColor(0));
	}
}


void KvPlottable1d::draw3d_(KvPaint* paint) const
{
	auto d = data();
	assert(d && d->dim() > 1 && d->isDiscreted());

	auto disc = std::dynamic_pointer_cast<KvDiscreted>(d);

	unsigned ch(0);
	auto getter = [&disc, &ch](unsigned i) -> KvPaint::point3 {
		auto pt = disc->pointAt(i, ch);
		return { pt[0], pt[1], pt[2] };
	};

	for (; ch < disc->channels(); ch++) 
		drawImpl_(paint, getter, disc->size(), majorColor(shareColor() ? 0 : ch));
}
