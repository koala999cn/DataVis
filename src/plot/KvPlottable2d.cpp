#include "KvPlottable2d.h"
#include "KvSampled.h"


const color4f& KvPlottable2d::minorColor() const
{
	return borderPen_.color;
}


void KvPlottable2d::setMinorColor(const color4f& minor)
{
	borderPen_.color = minor;
}


void KvPlottable2d::drawDiscreted_(KvPaint* paint, KvDiscreted* disc) const
{
	auto samp = dynamic_cast<KvSampled*>(disc);
	assert(samp && samp->dim() >= 2);

	if (useDefaultZ_)
		draw1d_(paint, samp);
	else
		draw2d_(paint, samp);
}


void KvPlottable2d::draw1d_(KvPaint* paint, KvSampled* samp) const
{
	auto z = defaultZ();

	unsigned ch(0);
	auto getter = [&samp, &ch, &z](unsigned ix, unsigned iy) -> KvPaint::point3 {
		auto pt = samp->point(ix, iy, ch);
		return { pt[0], pt[1], z };
	};

	for (; ch < samp->channels(); ch++) {
		drawImpl_(paint, getter, samp->size(0), samp->size(1), ch);
		z += stepZ();
	}
}


void KvPlottable2d::draw2d_(KvPaint* paint, KvSampled* samp) const
{
	unsigned ch(0);
	auto getter = [&samp, &ch](unsigned ix, unsigned iy) -> KvPaint::point3 {
		auto pt = samp->point(ix, iy, ch);
		return { pt[0], pt[1], pt[2] };
	};

	for (; ch < samp->channels(); ch++) 
		drawImpl_(paint, getter, samp->size(0), samp->size(1), ch);
}
