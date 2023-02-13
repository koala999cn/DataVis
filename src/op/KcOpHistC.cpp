#include "KcOpHistC.h"
#include "KtSampling.h"
#include <vector>
#include "KcSampled1d.h"
#include "KgHistC.h"
#include "imgui.h"
#include "stlex.h"
#include "KuMath.h"
#include "KuDataUtil.h"


KcOpHistC::KcOpHistC()
    : super_("HistC", true, true)
{
    bins_ = 99;
    low_ = 0, high_ = 1;
}


int KcOpHistC::spec(kIndex outPort) const
{
    KpDataSpec ds(super_::spec(outPort));
    ds.dynamic = ds.stream;
    ds.stream = false;
    ds.dim = 1;
    ds.type = k_sampled; // TODO: 暂时bin使用固定宽度
    
    return ds.spec;
}


kRange KcOpHistC::range(kIndex outPort, kIndex axis) const
{
    if (axis == 0)
        return { low_, high_ };

    assert(axis == 1);
    return { 0, 1 }; // TODO: 暂时使用归一化范围，应根据histc类型调整
}


kReal KcOpHistC::step(kIndex outPort, kIndex axis) const
{
    assert(low_ != high_);

    if (axis == 0) {
        KtSampling<kReal> samp;
        samp.resetn(bins_, low_, high_, 0.5);
        return samp.dx();
    }

    return KvDiscreted::k_nonuniform_step;
}


kIndex KcOpHistC::size(kIndex outPort, kIndex axis) const
{
    return bins_;
}


bool KcOpHistC::permitInput(int dataSpec, unsigned inPort) const
{
    KpDataSpec ds(dataSpec);
    return ds.type != k_continued;
}


kIndex KcOpHistC::osize_(kIndex) const
{
    return bins_;
}


bool KcOpHistC::onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins)
{
    assert(histc_ .empty());

    if (!super_::onStartPipeline(ins))
        return false;

    if (low_ == high_) {
        auto r = inputRange_(dim(0));
        low_ = r.low(), high_ = r.high(); // 进一步同步值域范围
        if (low_ == high_)
            return false;
    }

    histc_.resize(channels(0));
    for (unsigned i = 0; i < channels(0); i++) {
        histc_[i] = std::make_unique<KgHistC>();
        if (histc_[i] == nullptr)
            return false;
        histc_[i]->resetLinear(bins_, low_, high_);
    }

    auto samp = std::make_shared<KcSampled1d>();
    samp->reset(0, low_, step(0, 0), 0.5);
    samp->resize(bins_, channels(0));
    odata_.front() = samp;

    return true;
}


void KcOpHistC::onStopPipeline()
{
    histc_.clear();
    super_::onStopPipeline();
}


void KcOpHistC::showPropertySet()
{
    super_::showPropertySet();
    ImGui::Separator();

    int bins(bins_);
    if (ImGui::DragInt("Hist Bins", &bins, 1, 1, 999999) && bins > 0) {
        bins_ = bins;
        setOutputExpired(0);
    }

    float low(low_), high(high_);
    if (ImGui::DragFloatRange2("Hist Range", &low, &high) && high > low) {
        low_ = low, high_ = high;
        setOutputExpired(0);
    }
}


void KcOpHistC::op_(const kReal* in, unsigned len, unsigned ch, kReal* out)
{
    KpDataSpec ds(inputSpec_());
    if (!ds.stream) { // 如果输入不是流式数据，重置hist计数
        for (auto& i : histc_)
            i->reset();
    }

    histc_[ch]->count(in, len);
    auto& res = histc_[ch]->result();

    std::copy(res.begin(), res.end(), out);
    KuMath::scaleTo<kReal>(out, bins_, 1);
}


bool KcOpHistC::onNewLink(KcPortNode* from, KcPortNode* to)
{
    if (!super_::onNewLink(from, to))
        return false;

    KpDataSpec ds(inputSpec_());
    auto r = inputRange_(ds.dim); // 输入数据的值域范围
    low_ = r.low(), high_ = r.high();
    if (low_ == high_)
        high_ = low_ + 1;

    return true;
}


bool KcOpHistC::prepareOutput_()
{
    if (isOutputExpired()) { // TODO: 应检测输入数据的通道数变化
        for(auto& i : histc_)
            i->resetLinear(bins_, low_, high_);

        auto samp = std::dynamic_pointer_cast<KcSampled1d>(odata_.front());
        samp->reset(0, low_, step(0, 0), 0.5);
        samp->resize(bins_, channels(0));
        return true;
    }

    return false;
}
