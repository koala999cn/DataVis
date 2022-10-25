#include "KgFbank.h"
#include <assert.h>
#include <functional>
#include "KtSampling.h"
#include "KuFreqUnit.h"
#include "KtuMath.h"


KgFbank::KgFbank(KgFbank&& fbank) noexcept
    : opts_(fbank.opts_)
    , firstIdx_(std::move(fbank.firstIdx_))
    , fc_(std::move(fbank.fc_))
    , weights_(std::move(fbank.weights_))
{
    
}


KgFbank::KgFbank(const KpOptions& opts)
    : opts_(opts)
{
    if (opts_.highFreq <= 0)
        opts_.highFreq = opts.sampleRate / 2; // 取奈奎斯特频率

    // 边界检测
    assert(opts_.lowFreq >= 0 && opts_.highFreq > opts_.lowFreq);
    assert(opts_.type >= k_linear && opts_.type <= k_erb);

    initWeights_();
}


unsigned KgFbank::idim() const
{
    return opts_.fftBins;
}


unsigned KgFbank::odim() const
{
    return opts_.numBanks;
}


void KgFbank::process(const double* in, double* out) const
{
    for (unsigned i = 0; i < opts_.numBanks; i++) 
        // 若weights_[i].size() == 0, dot返回0.0
        out[i] = KtuMath<double>::dot(in + firstIdx_[i], weights_[i].data(), 
            static_cast<unsigned>(weights_[i].size()));
}


double KgFbank::toHertz_(double scale)
{
    static std::function<double(double)> cvt[] = {
        [](double f) { return f; }, // k_linear
        [](double f) { return exp(f) - 1; }, // k_log
        [](double f) { return KuFreqUnit::melToHertz(f); }, // k_mel
        [](double f) { return KuFreqUnit::barkToHertz(f); }, // k_bark
        [](double f) { return KuFreqUnit::camToHertz(f); }  // k_erb
    };

    return cvt[opts_.type](scale);
}


double KgFbank::fromHertz_(double hz)
{
    static std::function<double(double)> cvt[] = {
        [](double hz) { return hz; }, // k_linear
        [](double hz) { return log(hz + 1); }, // k_log
        [](double hz) { return KuFreqUnit::hertzToMel(hz); }, // k_mel
        [](double hz) { return KuFreqUnit::hertzToBark(hz); }, // k_bark
        [](double hz) { return KuFreqUnit::hertzToCam(hz); }  // k_erb
    };

    return cvt[opts_.type](hz);
}


void KgFbank::initWeights_()
{
    // Hz尺度的采样参数
    KtSampling<double> sanpHertz;
    sanpHertz.resetn(idim() - 1, 0, opts_.sampleRate / 2, 0); // 兼容kaldi, x0ref取0, n取idim - 1

    // 目标(type_)尺度的采样参数
    auto lowScale = fromHertz_(opts_.lowFreq);
    auto highScale = fromHertz_(opts_.highFreq);
    KtSampling<double> sanpScale;
    sanpScale.resetn(opts_.numBanks + 1, lowScale, highScale, 0); // 在目标尺度上均匀划分各bin，相邻的bin有1/2重叠

    firstIdx_.resize(opts_.numBanks);
    fc_.resize(opts_.numBanks);
    weights_.resize(opts_.numBanks);
    for (unsigned bin = 0; bin < opts_.numBanks; bin++) {

        // 计算当前bin在目标尺度上的规格（左边频率，右边频率，中心频率）
        auto fl = sanpScale.indexToX(bin);
        auto fc = sanpScale.indexToX(bin + 1);
        auto fr = sanpScale.indexToX(bin + 2);

        // 计算当前bin的频点范围
        auto flhz = toHertz_(fl); 
        auto frhz = toHertz_(fr);
        auto lowIdx = sanpHertz.xToHighIndex(flhz);
        auto highIdx = sanpHertz.xToLowIndex(frhz);
        lowIdx = std::max(lowIdx, long(0));
        highIdx = std::min(highIdx, long(idim() - 1));
        firstIdx_[bin] = lowIdx;
        fc_[bin] = toHertz_(fc);

        // 计算当前bin的权值
        weights_[bin].clear();
        if (lowIdx <= highIdx) {
            // 计算当前bin的权值数组（目标尺度上的三角滤波）
            auto& wt = weights_[bin];
            wt.resize(highIdx - lowIdx + 1, 0);
            for (long i = lowIdx; i <= highIdx; i++)
                wt[i - lowIdx] = calcFilterWeight_(fl, fr, fromHertz_(sanpHertz.indexToX(i)));

            if (opts_.normalize)
                KtuMath<double>::scale(wt.data(), 
                    static_cast<unsigned>(wt.size()), 1. / (frhz - flhz));
        }
    }
}


double KgFbank::calcFilterWeight_(double low, double high, double f)
{
    auto half = 0.5 * (high - low);
    return f <= low + half ? (f - low) / half : (high - f) / half;
}


const char* KgFbank::type2Str(KeType type)
{
    switch (type) {
    case k_linear:	return "power";
    case k_log:		return "log";
    case k_mel:		return "mel";
    case k_bark:	return "bark";
    case k_erb:		return "erb";
    default:		return "unknown";
    }
}


KgFbank::KeType KgFbank::str2Type(const char* str)
{
    if (0 == _stricmp(str, type2Str(k_linear)))
        return k_linear;

    if (0 == _stricmp(str, type2Str(k_log)))
        return k_log;

    if (0 == _stricmp(str, type2Str(k_mel)))
        return k_mel;

    if (0 == _stricmp(str, type2Str(k_bark)))
        return k_bark;

    if (0 == _stricmp(str, type2Str(k_erb)))
        return k_erb;

    return k_mel;
}