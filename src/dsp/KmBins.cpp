#include "KmBins.h"
#include "KtuMath.h"
#include <assert.h>


void KmBins::resetLinear(kIndex numBins, kReal low, kReal high)
{
	assert(numBins > 0);
	bins_.resize(numBins + 1);
	bins_[0] = low; bins_.back() = high;

	if (numBins > 0) {
		kReal bw = (high - low) / numBins;
		for (kIndex i = 1; i < numBins; i++)
			bins_[i] = low + i * bw;
	}
}


void KmBins::resetLog(kIndex numBins, kReal low, kReal high)
{
	assert(numBins > 0);
	bins_.resize(numBins + 1);
	bins_[0] = low; bins_.back() = high;

	if (numBins > 0) {
		auto lowLog = std::log(low);
		auto highLow = std::log(high);
		kReal bw = (highLow - lowLog) / numBins;
		for (kIndex i = 1; i < numBins; i++)
			bins_[i] = std::exp(lowLog + i * bw);
	}
}
