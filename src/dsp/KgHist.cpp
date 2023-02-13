#include "KgHist.h"
#include <assert.h>
#include "KcSampled1d.h"
#include "KuMath.h"


void KgHist::process(const KvSampled& in, kReal* out)
{
    assert(numBins() > 0);

 /*   KtSampling<kReal> samp;
    samp.resetn(numBins(), range().first, range().second, 0.5);
    assert(KuMath::almostEqual(samp.dx(), binWidth(0))); // 假定线性尺度

    out.resize(numBins(), in.channels());
    out.reset(0, samp.low(), samp.dx(), samp.x0ref());
    
    process(in, (kReal*)out.data());*/

    if (in.empty()) {
        std::fill(out, out + numBins() * in.channels(), 0);
        return;
    }

    for (kIndex i = 0; i < numBins(); i++) {
        auto left = in.xToHighIndex(binLeft(i));
        auto right = in.xToHighIndex(binRight(i)); // 右侧为开区间
        assert(left <= right);

        if (left < 0) left = 0;
        if (right > in.size()) right = in.size();

        kIndex N = right - left;
        for (kIndex ch = 0; ch < in.channels(); ch++) {
            kReal val(0);  
            for (kIndex i = left; i < right; i++)
                val += in.value(i, ch);
            *out++ = val / N;
        }
    }
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
                KuMath::scale(out, in.channels(), kReal(1) / c);

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