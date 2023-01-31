#include "KcOpFIR.h"
#include "dsp/KcSampled1d.h"
#include "kfr/dsp.hpp"
#include <assert.h>
#include "imgui.h"


KcOpFIR::KcOpFIR()
    : super_("FIR")
{
    type_ = 0;
    window_ = 0;
    taps_ = 127;
    cutoff0_ = 0.2;
    cutoff1_ = 0.4;
    dirty_ = true;
}


bool KcOpFIR::onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins)
{
    createFilter_();
    return filter_ != nullptr;
}


void KcOpFIR::onStopPipeline()
{
    filter_.reset();
}


void KcOpFIR::outputImpl_()
{
    if (dirty_) {
        createFilter_();
        dirty_ = false;
    }

    assert(dim(0) == 1 && isSampled(0));
    assert(filter_ && filter_->channels() == idata_.front()->channels());
    auto samp1d = std::dynamic_pointer_cast<KcSampled1d>(idata_.front());
    assert(samp1d);

    auto res = std::make_shared<KcSampled1d>();
    res->resize(samp1d->size(), samp1d->channels());
    res->reset(0, samp1d->range(0).low(), samp1d->step(0));
    auto sz = filter_->apply(samp1d->data(), samp1d->size(), res->data());
    assert(sz <= samp1d->size());
    res->resize(sz); // TODO: 处理此种情况
    odata_.front() = res;
}


bool KcOpFIR::permitInput(int dataSpec, unsigned inPort) const
{
    KpDataSpec ds(dataSpec);
    return ds.dim == 1 && ds.type == k_sampled;
}


namespace kPrivate
{
    enum KeFilterType
    {
        k_lowpass,
        k_highpass,
        k_bandpass,
        k_bandstop
    };

    enum KeWindowType
    {
        k_kaiser,
        k_blackman_harris,
        k_hann
    };
}


void KcOpFIR::showProperySet()
{
    super_::showProperySet();
    ImGui::Separator();

    static const char* type[] = {
        "lowpass", 
        "highpass", 
        "bandpass", 
        "bandstop"
    };
    if (ImGui::BeginCombo("Type", type[type_])) {
        for (unsigned i = 0; i < std::size(type); i++)
            if (ImGui::Selectable(type[i], i == type_)) {
                type_ = i;
                dirty_ = true;
            }
        ImGui::EndCombo();
    }

    static const char* win[] = {
        "Kaiser",
        "Blackman Harris",
        "Hann"
    };
    if (ImGui::BeginCombo("Window", win[window_])) {
        for (unsigned i = 0; i < std::size(win); i++)
            if (ImGui::Selectable(win[i], i == window_)) {
                window_ = i;
                dirty_ = true;
            }
        ImGui::EndCombo();
    }

    if (ImGui::DragInt("Tap", &taps_, 1, 1, 1e10))
        dirty_ = true;

    if (type_ > 1) {
        if (ImGui::DragFloatRange2("Cutoff", &cutoff0_, &cutoff1_, 0.1, 0, 0.5))
            dirty_ = true;
    }
    else {
        if (ImGui::DragFloat("Cutoff", &cutoff0_, 0.1, 0, 0.5))
            dirty_ = true;
    }
}



void KcOpFIR::createFilter_()
{
    using namespace kfr;

    // Initialize window function
    expression_pointer<kReal> win;
    switch (window_)
    {
    case kPrivate::k_kaiser:
        win = to_pointer(window_kaiser<kReal>(taps_, 3.0));
        break;

    case kPrivate::k_hann:
        win = to_pointer(window_hann<kReal>(taps_));
        break;

    case kPrivate::k_blackman_harris:
        win = to_pointer(window_blackman_harris<kReal>(taps_));
        break;
    } 


    // Initialize taps
    std::vector<kReal> taps(taps_);
    auto univ = make_univector(taps);
    switch (type_)
    {
    case kPrivate::k_lowpass:
        fir_lowpass(univ, cutoff0_, win, true);
        break;

    case kPrivate::k_highpass:
        fir_highpass(univ, cutoff0_, win, true);
        break;

    case kPrivate::k_bandpass:
        fir_bandpass(univ, cutoff0_, cutoff1_, win, true);
        break;

    case kPrivate::k_bandstop:
        fir_bandstop(univ, cutoff0_, cutoff1_, win, true);
        break;
    }
        
    // Initialize filter and delay line
    filter_.reset(new KtFIR<kReal>(taps, channels(0)));
}
