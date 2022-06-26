#include "KcOpFilterBank.h"
#include "KgFilterBank.h"
#include "KcSampled1d.h"
#include "KcSampled2d.h"
#include <QPointF>


KcOpFilterBank::KcOpFilterBank(KvDataProvider* prov)
	: KvOpHelper1d("FilterBank", prov)
{
	fbank_ = std::make_unique<KgFilterBank>();
    auto r = prov->range(prov->dim() - 1);
    low_ = r.low();
    high_ = r.high();
    df_ = prov->step(prov->dim() - 1);
    fbank_->reset(KgFilterBank::k_mel, 13, df_, low_, high_);
}


kRange KcOpFilterBank::range(kIndex axis) const
{
    auto objp = dynamic_cast<const KvDataProvider*>(parent());
    assert(objp != nullptr);

    if (axis == objp->dim() - 1)
        return { low_, high_ };

    return KvDataOperator::range(axis); // TODO: 可以更精确计算
}


kReal KcOpFilterBank::step(kIndex axis) const
{
    auto objp = dynamic_cast<const KvDataProvider*>(parent());
    assert(objp != nullptr);

    if (axis == objp->dim() - 1)
        return range(axis).length() / size(axis); // TODO: nonuniform

    return KvDataOperator::step(axis);
}


kIndex KcOpFilterBank::size(kIndex axis) const
{
    auto objp = dynamic_cast<const KvDataProvider*>(parent());
    assert(objp != nullptr);

    if (axis == objp->dim() - 1)
        return fbank_->numBins();

    return KvDataOperator::size(axis);
}


namespace kPrivate
{
    enum KeFBankPropertyId
    {
        k_type,
        k_bands,
        k_range
    };
}


KcOpFilterBank::kPropertySet KcOpFilterBank::propertySet() const
{
    kPropertySet ps;

    KpProperty prop;
    KpProperty subProp;

    static const std::pair<QString, int> type[] = {
        { "Linear", KgFilterBank::k_linear },
        { "Log", KgFilterBank::k_log },
        { "Mel", KgFilterBank::k_mel },
        { "Bark", KgFilterBank::k_bark },
        { "Erb", KgFilterBank::k_erb },
    };

    prop.id = kPrivate::k_type;
    prop.name = tr("Type");
    prop.val = fbank_->type();
    for (unsigned i = 0; i < sizeof(type) / sizeof(std::pair<QString, int>); i++) {
        subProp.name = type[i].first;
        subProp.val = type[i].second;
        prop.children.push_back(subProp);
    }
    ps.push_back(prop);
    prop.children.clear();

    prop.id = kPrivate::k_bands;
    prop.name = tr("Bins");
    prop.val = fbank_->numBins();
    prop.minVal = 3;
    prop.step = 1;
    prop.maxVal = 99;
    ps.push_back(prop);

    auto objp = dynamic_cast<const KvDataProvider*>(parent());
    auto r = objp->range(objp->dim() - 1);
    prop.id = kPrivate::k_range;
    prop.name = tr("Range");
    prop.flag = KvPropertiedObject::k_restrict;
    prop.val = QPointF(low_, high_);
    subProp.name = tr("low");
    subProp.minVal = r.low();
    prop.children.push_back(subProp);
    subProp.name = tr("high");
    subProp.maxVal = r.high();
    prop.children.push_back(subProp);
    ps.push_back(prop);

    return ps;
}


void KcOpFilterBank::setPropertyImpl_(int id, const QVariant& newVal)
{
    KvDataProvider* objp = dynamic_cast<KvDataProvider*>(parent());

    switch (id)
    {
    case kPrivate::k_type:
        fbank_->reset(newVal.toInt(), fbank_->numBins(), df_, low_, high_);
        break;

    case kPrivate::k_bands:
        fbank_->reset(fbank_->type(), newVal.toInt(), df_, low_, high_);
        break;

    case kPrivate::k_range:
        low_ = newVal.value<QPointF>().x();
        high_ = newVal.value<QPointF>().y();
        fbank_->reset(fbank_->type(), fbank_->numBins(), df_, low_, high_);
        break;
    }
}


void KcOpFilterBank::preRender_()
{
    KvDataProvider* objp = dynamic_cast<KvDataProvider*>(parent());
    if (objp->step(objp->dim() - 1) != df_) {
        df_ = objp->step(objp->dim() - 1);
        fbank_->reset(fbank_->type(), fbank_->numBins(), df_, low_, high_);
    }
}


void KcOpFilterBank::processNaive_(const kReal* in, unsigned len, kReal* out)
{
    fbank_->process(in, len, out);
}
