#include "KcOpResampler.h"
#include "dsp/KgResampler.h"
#include "KcSampled1d.h"


KcOpResampler::KcOpResampler(KvDataProvider* prov)
    : KvDataOperator("resampler", prov)
{
    factor_ = 0.5;
    method_ = KgResampler::k_linear;
    winlen_ = 2;

    resamp_ = std::make_unique<KgResampler>(method_, winlen_, prov->channels(), factor_);
}


kReal KcOpResampler::step(kIndex axis) const
{
    assert(axis < dim());
    auto st = KvDataOperator::step(axis);
    if (axis == dim() - 1)
        st /= factor_;
    return st;
}


kIndex KcOpResampler::size(kIndex axis) const
{
    assert(axis < dim());
    auto sz = KvDataOperator::size(axis);
    if (axis == dim() - 1)
        sz *= factor_;
    return sz;
}


namespace kPrivate
{
    enum KeResamplerProperty
    {
        k_factor,
        k_interp_method,
        k_window_length
    };
}


KcOpResampler::kPropertySet KcOpResampler::propertySet() const
{
    using namespace kPrivate;

    kPropertySet ps;

    KpProperty prop;

    prop.id = k_factor;
    prop.name = tr("Factor");
    prop.val = factor_;
    prop.minVal = 0.001;
    prop.maxVal = 1024;
    prop.step = 0.1;
    ps.push_back(prop);

    static const std::pair<QString, int> method[] = {
        { "linear", KgResampler::k_linear },
        { "lagrange", KgResampler::k_lagrange },
        { "sinc", KgResampler::k_sinc }
    };
    prop.id = k_interp_method;
    prop.name = u8"Method";
    prop.val = method_;
    for (unsigned i = 0; i < sizeof(method) / sizeof(std::pair<QString, int>); i++) {
        KvPropertiedObject::KpProperty sub;
        sub.name = method[i].first;
        sub.val = method[i].second;
        prop.children.push_back(sub);
    }
    ps.push_back(prop);

    prop.id = k_window_length;
    prop.name = tr("WindowLength");
    prop.val = winlen_;
    prop.minVal = 2;
    prop.maxVal = 1024 * 16; // 16k
    prop.step = 2;
    prop.children.clear();
    ps.push_back(prop);

    return ps;
}



void KcOpResampler::setPropertyImpl_(int id, const QVariant& newVal)
{
    switch (id) {
    case kPrivate::k_factor:
        factor_ = newVal.value<kReal>();
        break;

    case kPrivate::k_interp_method:
        method_ = newVal.toInt();
        break;

    case kPrivate::k_window_length:
        winlen_ = newVal.toInt();
        break;
    }
}


void KcOpResampler::syncParent()
{
    assert(resamp_);

    if (resamp_->factor() != factor_ ||
        resamp_->length() != winlen_ || 
        resamp_->method() != method_) {
        auto objp = dynamic_cast<KvDataProvider*>(parent());
        assert(objp);
        resamp_->reset(method_, winlen_, objp->channels(), factor_);
    }
}


std::shared_ptr<KvData> KcOpResampler::processImpl_(std::shared_ptr<KvData> data)
{
    assert(data->isDiscreted() && data->dim() == 1);
    auto samp1d = std::dynamic_pointer_cast<KcSampled1d>(data);
    assert(samp1d);

    auto res = std::make_shared<KcSampled1d>();
    res->reset(0, samp1d->range(0).low(), step(0), samp1d->sampling(0).x0ref()); // TODO: 处理延时
    res->resize(resamp_->olength(samp1d->count()), samp1d->channels());

    auto N = resamp_->apply(samp1d->data(), samp1d->count(), res->data(), res->count());
    assert(N <= res->count());
    if(N != res->count())
        res->resize(N);
    return res;
}


