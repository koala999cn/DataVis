#include "KvPlottable1d.h"
#include "KvDiscreted.h"
#include "KvContinued.h"
#include "KvPaint.h"
#include "KtSampling.h"
#include "KtSampler.h"

void KvPlottable1d::drawDiscreted_(KvPaint* paint, KvDiscreted* disc) const
{
	if (disc->dim() == 1)
		draw1d_(paint, disc);
	else if (disc->isSampled())
		draw2d_(paint, disc);
	else
		draw3d_(paint, disc);
}


void KvPlottable1d::draw1d_(KvPaint* paint, KvDiscreted* disc) const
{
	auto defaultZ = defaultZ_;

	unsigned ch(0);
	auto getter = [&disc, &ch, &defaultZ](unsigned i) -> KvPaint::point3 {
		auto pt = disc->pointAt(i, ch);
		return { pt[0], pt[1], defaultZ };
	};

	for (; ch < disc->channels(); ch++) {
		drawImpl_(paint, getter, disc->size(), ch);
		defaultZ += stepZ_;
	}
}


void KvPlottable1d::draw2d_(KvPaint* paint, KvDiscreted* disc) const
{
	assert(disc->isSampled() && disc->dim() == 2);

	// TODO: 暂时只处理第1个通道
	assert(disc->channels() == 1);

	kIndex rowIdx;
	auto getter = [&disc, &rowIdx](unsigned i) -> KvPaint::point3 {
		auto n = rowIdx * disc->size(1) * disc->channels() + i;
		auto pt = disc->pointAt(n, 0);
		return { pt[0], pt[1], pt[2] };
	};

	for (kIndex ix = 0; ix < disc->size(0); ix++) {
		rowIdx = ix;
		drawImpl_(paint, getter, disc->size(1), 0);
	}
}


void KvPlottable1d::draw3d_(KvPaint* paint, KvDiscreted* disc) const
{
	unsigned ch(0);
	auto getter = [&disc, &ch](unsigned i) -> KvPaint::point3 {
		auto pt = disc->pointAt(i, ch);
		return { pt[0], pt[1], pt[2] };
	};

	for (; ch < disc->channels(); ch++) 
		drawImpl_(paint, getter, disc->size(), ch);
}
