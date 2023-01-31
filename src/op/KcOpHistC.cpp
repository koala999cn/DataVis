﻿#include "KcOpHistC.h"
#include "KtSampling.h"
#include <vector>
#include "KcSampled1d.h"
#include "KgHistC.h"
#include "imgui.h"
#include "stlex.h"
#include "KuMath.h"


KcOpHistC::KcOpHistC()
    : super_("HistC")
{
    bins_ = 9;;
    low_ = 0, high_ = 1;
}


int KcOpHistC::spec(kIndex outPort) const
{
    KpDataSpec ds(super_::spec(outPort));
    ds.dynamic = ds.stream;
    ds.stream = false;
    ds.dim = 1;
    ds.type = k_sampled;
    
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
    if (axis == 0 && low_ != high_) {
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


bool KcOpHistC::onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins)
{
    assert(histc_ == nullptr);

    if (low_ == high_) {
        auto r = inputRange_(dim(0));
        low_ = r.low(), high_ = r.high(); // 进一步同步值域范围
        if (low_ == high_)
            return false;
    }

    histc_ = std::make_unique<KgHistC>();
    if (histc_ == nullptr)
        return false;
    histc_->resetLinear(bins_, low_, high_);

    auto samp = std::make_shared<KcSampled1d>();
    if (samp == nullptr) {
        histc_.reset();
        return false;
    }

    samp->reset(0, histc_->range().first, histc_->binWidth(0), 0.5);
    samp->resize(histc_->numBins(), channels(0));
    odata_.front() = samp;
    return true;
}


void KcOpHistC::onStopPipeline()
{
    histc_.reset();
}


void KcOpHistC::showProperySet()
{
    super_::showProperySet();
    ImGui::Separator();

    if (ImGui::DragInt("Hist Bins", &bins_, 1, 1, 999999))
        notifyChanged_();

    if (ImGui::DragFloatRange2("Hist Range", &low_, &high_)) 
        notifyChanged_();
}


void KcOpHistC::outputImpl_()
{
    KpDataSpec ds(inputSpec_());
    if (!ds.stream) // 如果输入不是流式数据，重置hist计数
        histc_->reset();

    auto disc = std::dynamic_pointer_cast<KvDiscreted>(idata_.front());
    assert(disc);

    std::vector<kReal> vals(disc->size());
    for (unsigned i = 0; i < disc->size(); i++)
        vals[i] = disc->valueAt(i, 0); // TODO: 多通道支持

    histc_->count(vals.data(), vals.size());
    auto& res = histc_->result();
    stdx::copy(res, vals);
    KuMath::scaleTo(vals.data(), vals.size(), 0.9);

    auto samp = std::dynamic_pointer_cast<KcSampled1d>(odata_.front());
    samp->setChannel(nullptr, 0, vals.data()); // TODO: 多通道支持
}


bool KcOpHistC::onNewLink(KcPortNode* from, KcPortNode* to)
{
    if (!super_::onNewLink(from, to))
        return false;

    auto r = inputRange_(dim(0)); // 输入的值域范围
    low_ = r.low(), high_ = r.high();
    return true;
}


void KcOpHistC::onNewFrame(int frameIdx)
{
    assert(histc_);

    if (histc_->numBins() != bins_ ||
        histc_->range().first != low_ ||
        histc_->range().second != high_) {
        histc_->resetLinear(bins_, low_, high_);
        auto samp = std::dynamic_pointer_cast<KcSampled1d>(odata_.front());
        samp->reset(0, histc_->range().first, histc_->binWidth(0), 0.5);
        samp->resize(histc_->numBins(), channels(0));
    }
}
