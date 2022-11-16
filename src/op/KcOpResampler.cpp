#include "KcOpResampler.h"
#include "dsp/KgResampler.h"
#include "KcSampled1d.h"
#include "imgui.h"


KcOpResampler::KcOpResampler()
    : super_("Resampler", false)
{
    factor_ = 0.5;
    method_ = KgResampler::k_linear;
    wsize_ = 2;
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
    assert(resamp_ == nullptr);
    resamp_ = std::make_unique<KgResampler>(method_, wsize_, channels(0), factor_);
    prepareOutput_();
    return resamp_ != nullptr && odata_.front() != nullptr;
}


void KcOpResampler::onStopPipeline()
{
    resamp_.reset();
}


void KcOpResampler::onNewFrame(int frameIdx)
{
    assert(resamp_);

    if (resamp_->factor() != factor_ ||
        resamp_->size() != wsize_ ||
        resamp_->method() != method_) {
        resamp_->reset(method_, wsize_, channels(0), factor_);

        auto samp = std::dynamic_pointer_cast<KvSampled>(odata_.front());
        odata_.front() = nullptr; // 确保step返回正确的值
        for (kIndex i = 0; i < dim(0); i++)
            samp->reset(i, samp->range(i).low(), step(0, i), 0);
        odata_.front() = samp;
    }
}


void KcOpResampler::showProperySet()
{
    super_::showProperySet();
    ImGui::Separator();

    float factor = factor_;
    if (ImGui::DragFloat("Factor", &factor, 0.1, 0.001, 1000))
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

    ImGui::DragInt("Window Length", &wsize_, 2, 2, 1024 * 16);
}


kIndex KcOpResampler::isize_() const
{
    return 0;
}


kIndex KcOpResampler::osize_(kIndex is) const
{
    return resamp_ ? resamp_->osize(is) : is * factor_;
}


void KcOpResampler::op_(const kReal* in, unsigned len, kReal* out)
{
    resamp_->apply(in, len, out, resamp_->osize(len));
}
