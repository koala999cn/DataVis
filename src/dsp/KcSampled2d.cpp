#include "KcSampled2d.h"

/*
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
	return { indexToValue(0, ix), indexToValue(1, iy), array_(int(ix), int(iy), int(channel)) };
}


void KcSampled2d::setChannel(kIndex row, const kReal* src, kIndex channel)
{
	assert(row >= 0 && row < length(0));
	assert(channel >= 0 && channel < channels());
	kReal* dst = at(row);
	if (channels() == 1) {
		std::copy(src, src + length(1), dst);
	}
	else {
		dst += channel * stride(2);
		for (kIndex col = 0; col < length(1); col++) {
			*dst = *src++;
			dst += stride(1);
		}
	}
}*/