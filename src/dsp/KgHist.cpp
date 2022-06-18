#include "KgHist.h"
#include <assert.h>
#include "KcSampled1d.h"
#include "KtuMath.h"


void KgHist::process(const KcSampled1d& in, KcSampled1d& out)
{
    assert(numBins() > 0);

    KtSampling<kReal> samp;
    samp.resetn(numBins(), range().first, range().second, 0.5);
    assert(KtuMath<kReal>::almostEqual(samp.dx(), binWidth(0))); // 假定线性尺度

    out.resize(numBins(), in.channels());
    out.reset(0, samp.low(), samp.dx(), samp.x0ref());
    
    process(in, (kReal*)out.data());
}


void KgHist::process(const KcSampled1d& in, kReal* out)
{
    for (kIndex i = 0; i < numBins(); i++) {
        auto range = in.sampling(0).rangeToIndex(binLeft(i), binRight(i));
        if (range.first > range.second) std::swap(range.first, range.second);
        if (range.first < 0) range.first = 0;
        if (range.second > in.size()) range.second = in.size();

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
    samp.resetn(len, range().first, range().second, 0.5);
}


void KgHist::process(const KvData& in, kReal* out)
{
    assert(numBins() > 0);
    assert(in.isDiscreted());

    const KvDiscreted& dis = (const KvDiscreted&)in;


    // 跳过统计区间（左）之外的数据点
    kIndex i = 0;
    while (i < in.size() && dis.pointAt(i, 0)[0] < binLeft(0))
        ++i;


    kIndex binIdx(0);
    auto barRight = binRight(binIdx);
    std::fill(out, out + numBins() * in.channels(), 0);
    unsigned c(0);

    while (i < in.size()) {
        if (dis.pointAt(i, 0)[0] < barRight) { // accumulate current bar
            for(kIndex ch = 0; ch < in.channels(); ch++)
                out[ch] += dis.valueAt(i, ch);
            ++i, ++c;
        }
        else { // goto next bar

            if (c > 0) 
                KtuMath<kReal>::scale(out, in.channels(), kReal(1) / c);

            if (barRight >= range().second)
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