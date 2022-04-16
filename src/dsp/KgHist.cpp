#include "KgHist.h"
#include <assert.h>
#include "KcSampled1d.h"
#include "KtuMath.h"


void KgHist::reset(kIndex numBins, kReal low, kReal high)
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


void KgHist::process(const KcSampled1d& in, KcSampled1d& out)
{
    assert(numBins() > 0);

    KtSampling<kReal> samp;
    samp.resetn(numBins(), range().first, range().second, 0.5);
    assert(KtuMath<kReal>::almostEqual(samp.dx(), binWidth(0))); // 假定线性尺度

    out.resize(numBins(), in.channels());
    out.reset(0, samp);
    
    process(in, (kReal*)out.data());
}


void KgHist::process(const KcSampled1d& in, kReal* out)
{
    for (kIndex i = 0; i < numBins(); i++) {
        auto range = in.sampling(0).rangeToIndex(bins_[i], bins_[i + 1]);
        if (range.first > range.second) std::swap(range.first, range.second);
        if (range.first < 0) range.first = 0;
        if (range.second > in.count()) range.second = in.count();

        for (kIndex c = 0; c < in.channels(); c++) {
            kReal val(0);
            kIndex N = range.second - range.first;
            for (kIndex j = range.first; j < range.second; j++)
                val += in.value(j, c);

            *out++ = val / N;
        }
    }
}


void KgHist::process(const kReal* in, unsigned len, kReal* out)
{
    KtSampling<kReal> samp;
    samp.resetn(len, bins_[0], bins_.back(), 0.5);
}


void KgHist::process(const KvData& in, kReal* out)
{
    assert(numBins() > 0);


    // 跳过统计区间（左）之外的数据点
    kIndex i = 0;
    while (i < in.count() && in.point(&i, 0)[0] < bins_[0])
        ++i;


    kIndex binIdx(0);
    auto barRight = binRight(binIdx);
    std::fill(out, out + numBins() * in.channels(), 0);
    unsigned c(0);

    while (i < in.count()) {
        if (in.point(&i, 0)[0] < barRight) { // accumulate current bar
            for(kIndex ch = 0; ch < in.channels(); ch++)
                out[ch] += in.value(&i, ch);
            ++i, ++c;
        }
        else { // goto next bar

            if (c > 0) 
                KtuMath<kReal>::scale(out, in.channels(), kReal(1) / c);

            if (barRight >= bins_.back())
                break;

            barRight = binRight(++binIdx);
            out += in.channels();
            c = 0;
        }
    }

    //if (c > 0) {
    //    sum /= c;
    //    res->addSamples(&sum, 1);
    //}
}