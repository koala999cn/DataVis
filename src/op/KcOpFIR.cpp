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
}


bool KcOpFIR::onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins)
{
    if (!super_::onStartPipeline(ins))
        return false;

    createFilter_();
    return filter_ != nullptr;
}


void KcOpFIR::onStopPipeline()
{
    filter_.reset();
    super_::onStopPipeline();
}


bool KcOpFIR::prepareOutput_()
{
    if (isOutputExpired() 
        || filter_->channels() == idata_.front()->channels()) {
        createFilter_();
        return true;
    }

    return false;
}


void KcOpFIR::outputImpl_()
{
    assert(dim(0) == 1 && isSampled(0));
    auto samp = std::dynamic_pointer_cast<KvSampled>(idata_.front());
    assert(samp); 

    auto res = std::make_shared<KcSampled1d>();
    res->resize(samp->size(), samp->channels());
    res->reset(0, samp->range(0).low(), samp->step(0));

    auto samp1d = std::dynamic_pointer_cast<KcSampled1d>(samp);
    if (samp1d) {
        auto sz = filter_->apply(samp1d->data(), samp1d->size(), res->data());
        assert(sz <= samp1d->size());
        res->resize(sz); // TODO: 处理此种情况
    }

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


void KcOpFIR::showPropertySet()
{
    super_::showPropertySet();
    ImGui::Separator();

    static const char* type[] = {
        "Low-Pass", 
        "High-Pass", 
        "Band-Pass", 
        "Band-Stop"
    };
    if (ImGui::Combo("Type", &type_, type, std::size(type)))
        setOutputExpired(0);

    static const char* win[] = {
        "Kaiser",
        "Blackman Harris",
        "Hann"
    };
    if (ImGui::Combo("Window", &window_, win, std::size(win)))
        setOutputExpired(0);

    if (ImGui::DragInt("Tap", &taps_, 1, 1, 1e10))
        setOutputExpired(0);

    if (type_ > 1) {
        if (ImGui::DragFloatRange2("Cutoff", &cutoff0_, &cutoff1_, 0.1, 0, 0.5))
            setOutputExpired(0);
    }
    else {
        float cutoff(cutoff0_); // cutoff不能为0，否则kfr构造fir时返回nan
        if (ImGui::DragFloat("Cutoff", &cutoff, 0.001, 1e-10, 0.5) && cutoff >= 1e-10) {
            cutoff0_ = cutoff;
            setOutputExpired(0);
        }
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
