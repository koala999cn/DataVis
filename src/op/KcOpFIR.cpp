#include "KcOpFIR.h"
#include "dsp/KcSampled1d.h"
#include <assert.h>
#include "QtAppEventHub.h"
#include "kfr/dsp.hpp"


KcOpFIR::KcOpFIR(KvDataProvider* prov)
    : KvDataOperator("fir", prov)
{
    type_ = 0;
    window_ = 0;
    taps_ = 127;
    cutoff0_ = 0.2;
    cutoff1_ = 0.4;
    dirty_ = true;
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

    enum KeFirPropertyId
    {
        k_type,
        k_window,
        k_taps,
        k_cutoff0,
        k_cutoff1
    };
}


KcOpFIR::kPropertySet KcOpFIR::propertySet() const
{
    kPropertySet ps;

    KpProperty prop;

    static const std::pair<QString, int> type[] = {
        { "lowpass", kPrivate::k_lowpass },
        { "highpass", kPrivate::k_highpass },
        { "bandpass", kPrivate::k_bandpass },
        { "bandstop", kPrivate::k_bandstop }
    };
    prop.id = kPrivate::k_type;
    prop.name = tr("Type");
    prop.val = type_;
    for (unsigned i = 0; i < sizeof(type) / sizeof(std::pair<QString, int>); i++) {
        KpProperty subProp;
        subProp.name = type[i].first;
        subProp.val = type[i].second;
        prop.children.push_back(subProp);
    }
    ps.push_back(prop);
    prop.children.clear();

    static const std::pair<QString, int> win[] = {
        { "Kaiser", kPrivate::k_kaiser },
        { "Blackman Harris", kPrivate::k_blackman_harris },
        { "Hann", kPrivate::k_hann }
    };
    prop.id = kPrivate::k_window;
    prop.name = tr("Window");
    prop.val = window_;
    for (unsigned i = 0; i < sizeof(win) / sizeof(std::pair<QString, int>); i++) {
        KpProperty subProp;
        subProp.name = win[i].first;
        subProp.val = win[i].second;
        prop.children.push_back(subProp);
    }
    ps.push_back(prop);
    prop.children.clear();

    prop.id = kPrivate::k_taps;
    prop.name = tr("Taps");
    prop.val = taps_;
    prop.minVal = 1;
    prop.maxVal = 1e10;
    prop.step = 1;
    ps.push_back(prop);

    bool cut2 = (type_ > 1);

    prop.id = kPrivate::k_cutoff0;
    prop.name = cut2 ? tr("Cutoff1") : tr("Cutoff");
    prop.val = cutoff0_;
    prop.minVal = 0;
    prop.maxVal = 0.5;
    prop.step = 0.1;
    ps.push_back(prop);

    if (cut2) {
        prop.id = kPrivate::k_cutoff1;
        prop.name = tr("Cutoff2");
        prop.val = cutoff1_;
        prop.minVal = 0;
        prop.maxVal = 0.5;
        prop.step = 0.1;
        ps.push_back(prop);
    }

    return ps;
}


void KcOpFIR::setPropertyImpl_(int id, const QVariant& newVal)
{
    switch(id) {
    case kPrivate::k_type:
        type_ = newVal.toInt();
        kAppEventHub->refreshPropertySheet(); // TODO: ø…”≈ªØ
        break;

    case kPrivate::k_window:
        window_ = newVal.toInt();
        break;

    case kPrivate::k_taps:
        taps_ = newVal.toInt();
        break;

    case kPrivate::k_cutoff0:
        cutoff0_ = newVal.value<kReal>();
        break;

    case kPrivate::k_cutoff1:
        cutoff1_ = newVal.value<kReal>();
        break;

    default:
        break;
    }

    dirty_ = true;
}


std::shared_ptr<KvData> KcOpFIR::processImpl_(std::shared_ptr<KvData> data)
{
    assert(data->dim() == 1 && data->isDiscreted());
    assert(filter_ && filter_->channels() == data->channels());
    auto samp1d = std::dynamic_pointer_cast<KcSampled1d>(data);
    assert(samp1d);

    auto res = std::make_shared<KcSampled1d>();
    res->resize(samp1d->count(), samp1d->channels());
    res->reset(0, samp1d->sampling(0));
    auto length = filter_->apply(samp1d->data(), samp1d->count(), res->data());
    res->resize(length);
    return res;
}


void KcOpFIR::syncParent()
{
    using namespace kfr;

    if (dirty_) {
        dirty_ = false;

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
        auto objp = dynamic_cast<const KvDataProvider*>(parent());
        assert(objp != nullptr);
        filter_.reset(new KtFIR<kReal>(taps, objp->channels()));
    }
}
