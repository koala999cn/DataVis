#include "KcSampled2d.h"


#if 0
namespace kPrivate
{
	using d_type = KtSampledN<kReal, 2>;
	using dptr_type = d_type*;
};


KcSampled2d::KcSampled2d()
{
	dptr_ = new kPrivate::d_type;
}


KcSampled2d::~KcSampled2d()
{
	delete static_cast<kPrivate::dptr_type*>(dptr_);
}


void KcSampled2d::resize(kIndex nx, kIndex ny, kIndex channles)
{
	//static_cast<kPrivate::dptr_type>(dptr_)->resize(nx, ny, channels);
}


kIndex KcSampled2d::length(kIndex axis) const
{
	return static_cast<kPrivate::dptr_type>(dptr_)->length(axis);
}


kRange KcSampled2d::range(kIndex axis) const
{
	if (axis < 2) {
		auto& r = static_cast<kPrivate::dptr_type>(dptr_)->range(axis);
		return { r.low(), r.high() };
	}

	return static_cast<kPrivate::dptr_type>(dptr_)->valueRange();
}


kReal KcSampled2d::step(kIndex axis) const
{
	return static_cast<kPrivate::dptr_type>(dptr_)->step(axis);
}


KcSampled2d::kPoint3d KcSampled2d::value(kIndex ix, kIndex iy, kIndex channel) const
{
	kReal x = static_cast<kPrivate::dptr_type>(dptr_)->indexToValue(0, ix);
	kReal y = static_cast<kPrivate::dptr_type>(dptr_)->indexToValue(1, ix);
	kReal z = static_cast<kPrivate::dptr_type>(dptr_)->at(int(ix), iy, channel);

	return { x, y, z };
}
#endif


void KcSampled2d::resize(kIndex nx, kIndex ny, kIndex c)
{
	array_.resizeAndPreserve(nx, ny, c);
}


KvData2d::kPoint3d KcSampled2d::value(kIndex ix, kIndex iy, kIndex channel) const
{
	return { samp_[0].indexToX(ix), samp_[1].indexToX(iy), array_(int(ix), int(iy), int(channel)) };
}