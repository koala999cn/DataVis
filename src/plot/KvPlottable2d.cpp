#include "KvPlottable2d.h"
#include "KvDiscreted.h"
#include "KvSampled.h"


KvPlottable2d::KvPlottable2d(const std::string_view& name)
	: super_(name)
{
	mapper_.setAt(0, color4f(0, 0, 0, 1));
	mapper_.setAt(1, color4f(1, 1, 1, 1));
}


unsigned KvPlottable2d::majorColorsNeeded() const
{
	return -1;
}


unsigned KvPlottable2d::majorColors() const
{
	return mapper_.numStops();
}


color4f KvPlottable2d::majorColor(unsigned idx) const
{
	return mapper_.stopAt(idx).second;
}


void KvPlottable2d::setMajorColors(const std::vector<color4f>& majors)
{
	mapper_.reset();

	std::vector<float_t> vals(majors.size());
	KtuMath<float_t>::linspace(0, 1, 0, vals.data(), majors.size());
	for (unsigned i = 0; i < majors.size(); i++)
		mapper_.setAt(vals[i], majors[i]);
}


bool KvPlottable2d::minorColorNeeded() const
{
	return 1;
}


color4f KvPlottable2d::minorColor() const
{
	return borderPen_.color;
}


void KvPlottable2d::setMinorColor(const color4f& minor)
{
	borderPen_.color = minor;
}


color4f KvPlottable2d::mapValueToColor_(float_t val) const
{
	auto factor = KtuMath<float_t>::remap<true>(val, mapLower_, mapUpper_);
	return mapper_.getAt(factor);
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
