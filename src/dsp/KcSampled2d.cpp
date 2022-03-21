#include "KcSampled2d.h"


void KcSampled2d::resize(kIndex nx, kIndex ny, kIndex c)
{
	array_.resizeAndPreserve(nx, ny, c);
}


void KcSampled2d::resize(const KvData2d& data)
{
	resize(data.length(0), data.length(1), data.channels());
	reset(0, data.range(0).low(), data.step(0));
	reset(1, data.range(1).low(), data.step(1));
}


KvData2d::kPoint3d KcSampled2d::value(kIndex ix, kIndex iy, kIndex channel) const
{
	return { samp_[0].indexToX(ix), samp_[1].indexToX(iy), array_(int(ix), int(iy), int(channel)) };
}


void KcSampled2d::setChannel(kIndex row, kReal* src, kIndex channel)
{
	assert(channel >= 0 && channel < channels());
	kReal* dst = at(row);
	for (kIndex col = 0; col < length(1); col++) {
		dst[channel] = *src++;
		dst += stride(1);
	}
}