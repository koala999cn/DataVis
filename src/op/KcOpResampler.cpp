#include "KcOpResampler.h"
#include "dsp/KgResampler.h"
#include "KcSampled1d.h"
#include "imapp/KsImApp.h"
#include "imapp/KgPipeline.h"
#include "imgui.h"
#include "KuMath.h"


KcOpResampler::KcOpResampler()
    : super_("Resampler", false, false)
{
    factor_ = 0.5;
    method_ = KgResampler::k_linear;
    wsize_ = 2;
    doFlush_ = false;
}


kReal KcOpResampler::step(kIndex outPort, kIndex axis) const
{
    assert(axis < dim(outPort));
    auto st = super_::step(outPort, axis);
    if (axis == dim(outPort) - 1 && odata_[outPort] == nullptr)
        st /= factor_;
    return st;
}


bool KcOpResampler::onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins)
{
    if (!super_::onStartPipeline(ins))
        return false;

    assert(resamp_ == nullptr);
    resamp_ = std::make_unique<KgResampler>(method_, wsize_, channels(0), factor_);

    createOutputData_();
    return resamp_ != nullptr && odata_.front() != nullptr;
}


void KcOpResampler::onStopPipeline()
{
    resamp_.reset();
    super_::onStopPipeline();
}


void KcOpResampler::showPropertySet()
{
    super_::showPropertySet();
    ImGui::Separator();

    float factor = factor_;
    if (ImGui::DragFloat("Factor", &factor, 0.1, 0.001, 1000) && factor > 0)
        factor_ = factor;

    static const char* method[] = {
        "linear", 
        "lagrange", 
        "sinc"
    };
    if (ImGui::BeginCombo("Method", method[method_])) {
        for (unsigned i = 0; i < std::size(method); i++)
            if (ImGui::Selectable(method[i], i == method_))
                method_ = i;
        ImGui::EndCombo();
    }

    ImGui::BeginDisabled(method_ == 0); 
    int linearWinSize = 2;
    auto isize = isize_();
    if (isize == 0) isize = 1024 * 16;
    if (ImGui::DragInt("Window Length", method_ == 0 ? &linearWinSize : &wsize_, 1, 2, isize))
        wsize_ = KuMath::clamp<int>(wsize_, 2, isize); // 保持正确的最小值
    ImGui::EndDisabled();

    if (!isStream(0))
        ImGui::Checkbox("Flush", &doFlush_);

}


kIndex KcOpResampler::isize_() const
{
    return 0;
}


kIndex KcOpResampler::osize_(kIndex is) const
{
    bool doFlush = doFlush_ && !isStream(0);
    auto halfwin = std::min<kIndex>(is, wsize_ - wsize_ / 2);
    return factor_ * (is + halfwin * (doFlush ? 0 : -1));
}


bool KcOpResampler::prepareOutput_()
{
    assert(resamp_);

    if (resamp_->factor() != factor_ ||
        resamp_->size() != wsize_ ||
        resamp_->method() != method_) { // 采样参数变化，重置采样器

        resamp_->reset(method_, wsize_, channels(0), factor_);

        auto samp = std::dynamic_pointer_cast<KvSampled>(odata_.front());
        odata_.front() = nullptr; // 确保step返回正确的值
        for (kIndex i = 0; i < dim(0); i++)
            samp->reset(i, samp->range(i).low(), step(0, i), 0);
        odata_.front() = samp;

        return true;
    }
    else if (!isStream(0)) { // 对于非流式数据，每帧都重置采样器
        resamp_->reset();
    }

    return false;
}


void KcOpResampler::op_(const kReal* in, unsigned ilen, kReal* out)
{
    auto olen = osize_(ilen);
    auto used = resamp_->apply(in, ilen, out, olen);
    if (!isStream(0) && doFlush_) {
        assert(resamp_->fsize() == olen - used);
        auto f = resamp_->flush(out + used * channels(0), olen - used);
        assert(f == olen - used);
    }
}
