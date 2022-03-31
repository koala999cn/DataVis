#include "KcWindowingOp.h"
#include "../dsp/KgWindowing.h"
#include "KcSampled1d.h"
#include "KcSampled2d.h"


KcWindowingOp::KcWindowingOp(KvDataProvider* prov)
    : KvDataOperator("windowing", prov)
    , type_(KuWindowFactory::k_hamming)
    , arg_(0)
{
    syncParent();
}


KcWindowingOp::kPropertySet KcWindowingOp::propertySet() const
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
    for (unsigned i = 0; i < sizeof(type) / sizeof(std::pair<QString, int>); i++) {
        subProp.name = type[i].first;
        subProp.val = type[i].second;
        prop.children.push_back(subProp);
    }
    ps.push_back(prop);

    return ps;
}


void KcWindowingOp::setPropertyImpl_(int id, const QVariant& newVal)
{
    assert(id == 0);
    type_ = newVal.toInt();
    KvDataProvider* objp = dynamic_cast<KvDataProvider*>(parent());
    assert(objp);
    if (type_ != -1)
        win_.reset(new KgWindowing(type_, objp->length(objp->dim() - 1), arg_));
    else
        win_.reset(nullptr);
}


void KcWindowingOp::syncParent()
{
    if (type_ >= 0) {
        KvDataProvider* objp = dynamic_cast<KvDataProvider*>(parent());
        assert(objp);
        if (win_ == nullptr || objp->length(objp->dim() - 1) != (*win_)->count()) 
            win_.reset(new KgWindowing(type_, objp->length(objp->dim() - 1), arg_));
    }
}


std::shared_ptr<KvData> KcWindowingOp::processImpl_(std::shared_ptr<KvData> data)
{
    if (win_ == nullptr)
        return data;

    assert(data->length(data->dim() - 1) == (*win_)->count());

    if (data->dim() == 1) {
        auto samp = std::dynamic_pointer_cast<KcSampled1d>(data);
        assert(samp);
        win_->process(*samp);
    }
    else if (data->dim() == 2) {
        auto samp = std::dynamic_pointer_cast<KcSampled2d>(data);
        assert(samp);
        for(kIndex i = 0; i < samp->length(0); i++)
            win_->process(samp->at(i), samp->channels());
    }
    else {
        assert(false);
    }

    return data;
}
