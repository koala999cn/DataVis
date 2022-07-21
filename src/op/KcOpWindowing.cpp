#include "KcOpWindowing.h"
#include "../dsp/KgWindowing.h"
#include "KcSampled1d.h"
#include "KcSampled2d.h"


KcOpWindowing::KcOpWindowing(KvDataProvider* prov)
    : KvDataOperator("windowing", prov)
    , type_(KuWindowFactory::k_hamming)
    , arg_(0)
{
    preRender_();
}


KcOpWindowing::kPropertySet KcOpWindowing::propertySet() const
{
    kPropertySet ps;

    KpProperty prop;
    KpProperty subProp;

    static const std::pair<QString, int> type[] = {
        { "None", -1 },
        { "Hamming", KuWindowFactory::k_hamming },
        { "Hann", KuWindowFactory::k_hann },
        { "Povey", KuWindowFactory::k_povey },
        { "Blackman", KuWindowFactory::k_blackman },
        { "Blackman Harris", KuWindowFactory::k_blackmanharris },
        { "Blackman Harris7", KuWindowFactory::k_blackmanharris7 },
        { "Flat Top", KuWindowFactory::k_flattop },
    };

    prop.id = 0;
    prop.name = tr("type");
    prop.val = type_;
    prop.makeEnum(type);
    ps.push_back(prop);

    return ps;
}


void KcOpWindowing::setPropertyImpl_(int id, const QVariant& newVal)
{
    assert(id == 0);
    type_ = newVal.toInt();
    KvDataProvider* objp = dynamic_cast<KvDataProvider*>(parent());
    assert(objp);
    if (type_ != -1)
        win_.reset(new KgWindowing(type_, objp->size(objp->dim() - 1), arg_));
    else
        win_.reset(nullptr);
}


void KcOpWindowing::preRender_()
{
    if (type_ >= 0) {
        KvDataProvider* objp = dynamic_cast<KvDataProvider*>(parent());
        assert(objp);
        if (win_ == nullptr || objp->size(objp->dim() - 1) != (*win_)->size())
            win_.reset(new KgWindowing(type_, objp->size(objp->dim() - 1), arg_));
    }
}


std::shared_ptr<KvData> KcOpWindowing::processImpl_(std::shared_ptr<KvData> data)
{
    if (win_ == nullptr)
        return data;


    if (data->dim() == 1) {
        auto samp = std::dynamic_pointer_cast<KcSampled1d>(data);
        assert(samp && samp->size(0) == (*win_)->size());
        win_->process(*samp);
    }
    else if (data->dim() == 2) {
        auto samp = std::dynamic_pointer_cast<KcSampled2d>(data);
        assert(samp && samp->size(1) == (*win_)->size());
        for(kIndex i = 0; i < samp->size(0); i++)
            win_->process(samp->row(i), samp->channels());
    }
    else {
        assert(false);
    }

    return data;
}
