#include "KgFilterBank.h"
#include <functional>
#include <assert.h>
#include "KcSampled1d.h"
#include "KuFreqUnit.h"
#include "KtuMath.h"


KgFilterBank::KgFilterBank(int type)
    : type_(type)
{
    
}


void KgFilterBank::reset(int type, kIndex numBins, kReal df, kReal lowFreq, kReal highFreq)
{
    // 边界检测
    assert(lowFreq >= 0 && highFreq > lowFreq);
    assert(type >= k_linear && type <= k_erb);

    type_ = type;

    // Hz尺度的采样参数
    KtSampling<kReal> sampHz;
    sampHz.reset(lowFreq, highFreq, df, 0.5);

    // 目标(type_)尺度的采样参数
    auto lowScale = fromHertz_(lowFreq);
    auto highScale = fromHertz_(highFreq);
    KtSampling<kReal> sampTy;
    sampTy.resetn(numBins + 1, lowScale, highScale, 0); // 在目标尺度上均匀划分各bin，相邻的bin有1/2重叠

    firstIdx_.resize(numBins);
    fc_.resize(numBins);
    weights_.resize(numBins);
    for (kIndex bin = 0; bin < numBins; bin++) {
        // 计算目标尺度上的bin参数（左边频率，右边频率，中心频率）
        auto fl = sampTy.indexToX(bin);
        auto fc = sampTy.indexToX(bin + 1);
        auto fr = sampTy.indexToX(bin + 2);

        // 换算到Hertz尺度上保存
        auto flhz = toHertz_(fl);
        auto frhz = toHertz_(fr);
        auto idx = sampHz.rangeToIndex(flhz, frhz);
        assert(idx.first >= 0);
        firstIdx_[bin] = idx.first;
        fc_[bin] = toHertz_(fc);

        if (idx.first < idx.second) {
            // 计算当前bin的权值数组（目标尺度上的三角滤波）
            auto factor = 1 / (frhz - flhz);
            weights_[bin].resize(idx.second - idx.first, 0);
            for (kIndex i = idx.first; i < idx.second; i++)
                weights_[bin][i - idx.first] = factor *
                calcFilterWeight_(fl, fr, fromHertz_(sampHz.indexToX(i)));
        }
        else {
            weights_[bin].clear();
        }
    }
}


void KgFilterBank::process(const KcSampled1d& in, kReal* out)
{
    //assert(in.count() == firstIdx_.back() + weights_.back().size());

    process(in.data(), in.count(), out); // TODO: 多通道。设计一种通用范式来处理
}


void KgFilterBank::process(const kReal* in, unsigned len, kReal* out)
{
    for (size_t i = 0; i < numBins(); i++) {
        int N = std::min<kIndex>(weights_[i].size(), int(len) - firstIdx_[i]);
        if (N < 0) N = 0;
        out[i] = KtuMath<kReal>::dot(in + firstIdx_[i], weights_[i].data(), N);
    }
}


kReal KgFilterBank::toHertz_(kReal scale)
{
    static std::function<kReal(kReal)> cvt[] = {
        [](kReal f) { return f; }, // k_linear
        [](kReal f) { return exp(f) - 1; }, // k_log
        [](kReal f) { return KuFreqUnit::melToHertz(f); }, // k_mel
        [](kReal f) { return KuFreqUnit::barkToHertz(f); }, // k_bark
        [](kReal f) { return KuFreqUnit::camToHertz(f); }  // k_erb
    };

    assert(type_ >= 0 && type_ < 5);
    return cvt[type_](scale);
}


kReal KgFilterBank::fromHertz_(kReal hz)
{
    static std::function<kReal(kReal)> cvt[] = {
        [](kReal hz) { return hz; }, // k_linear
        [](kReal hz) { return log(hz + 1); }, // k_log
        [](kReal hz) { return KuFreqUnit::hertzToMel(hz); }, // k_mel
        [](kReal hz) { return KuFreqUnit::hertzToBark(hz); }, // k_bark
        [](kReal hz) { return KuFreqUnit::hertzToCam(hz); }  // k_erb
    };

    assert(type_ >= 0 && type_ < 5);
    return cvt[type_](hz);
}


kReal KgFilterBank::calcFilterWeight_(kReal low, kReal high, kReal f)
{
    auto half = 0.5 * (high - low);
    return f <= low + half ? (f - low) / half : (high - f) / half;
}