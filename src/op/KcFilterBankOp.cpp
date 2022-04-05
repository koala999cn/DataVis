#include "KcFilterBankOp.h"
#include "KgFilterBank.h"
#include "KcSampled1d.h"
#include "KcSampled2d.h"
#include <QPointF>


KcFilterBankOp::KcFilterBankOp(KvDataProvider* prov)
	: KvDataOperator("FilterBank", prov)
{
	fbank_ = std::make_unique<KgFilterBank>();
    auto r = prov->range(prov->dim() - 1);
    low_ = r.low();
    high_ = r.high();
    df_ = prov->step(prov->dim() - 1);
    fbank_->reset(KgFilterBank::k_mel, 13, df_, low_, high_);
}


kRange KcFilterBankOp::range(kIndex axis) const
{
    auto objp = dynamic_cast<const KvDataProvider*>(parent());
    assert(objp != nullptr);

    if (axis == objp->dim() - 1)
        return { low_, high_ };

    return KvDataOperator::range(axis); // TODO: 可以更精确计算
}


kReal KcFilterBankOp::step(kIndex axis) const
{
    auto objp = dynamic_cast<const KvDataProvider*>(parent());
    assert(objp != nullptr);

    if (axis == objp->dim() - 1)
        return range(axis).length() / length(axis); // TODO: nonuniform

    return KvDataOperator::step(axis);
}


kIndex KcFilterBankOp::length(kIndex axis) const
{
    auto objp = dynamic_cast<const KvDataProvider*>(parent());
    assert(objp != nullptr);

    if (axis == objp->dim() - 1)
        return fbank_->numBins();

    return KvDataOperator::length(axis);
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


KcFilterBankOp::kPropertySet KcFilterBankOp::propertySet() const
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


void KcFilterBankOp::setPropertyImpl_(int id, const QVariant& newVal)
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


std::shared_ptr<KvData> KcFilterBankOp::processImpl_(std::shared_ptr<KvData> data)
{
	assert(data->channels() == 1);

    if (data->dim() == 1) {
        std::shared_ptr<KcSampled1d> res = std::make_shared<KcSampled1d>();
        res->reset(step(0), data->channels(), fbank_->numBins()); // TODO: nonuniform
        auto data1d = std::dynamic_pointer_cast<KcSampled1d>(data);
        fbank_->process(*data1d, (kReal*)res->data());
        return res;
    }
   
    return process2d_(data);
}


void KcFilterBankOp::syncParent()
{
    KvDataProvider* objp = dynamic_cast<KvDataProvider*>(parent());
    if (objp->step(objp->dim() - 1) != df_) {
        df_ = objp->step(objp->dim() - 1);
        fbank_->reset(fbank_->type(), fbank_->numBins(), df_, low_, high_);
    }
}


std::shared_ptr<KvData> KcFilterBankOp::process2d_(std::shared_ptr<KvData> data)
{
    assert(data->dim() == 2);
    assert(fbank_->numBins() == length(1));

    auto data2d = std::dynamic_pointer_cast<KvData2d>(data);
    assert(data2d && data->step(1) == df_);

    if (data2d->length(1) < 2 || data2d->range(1).empty())
        return data;

    auto res = std::make_shared<KcSampled2d>();

    res->resize(data2d->length(0), length(1), data2d->channels());
    res->reset(0, range(0).low(), step(0));
    res->reset(1, range(1).low(), step(1)); // TODO: nonuniform

    std::vector<kReal> rawData(data2d->length(1));
    std::vector<kReal> out(length(1));
    for (kIndex c = 0; c < data2d->channels(); c++) {
        for (kIndex i = 0; i < data2d->length(0); i++) {
            for (kIndex j = 0; j < data2d->length(1); j++)
                rawData[j] = data2d->value(i, j, c).z;

            fbank_->process(rawData.data(), rawData.size(), out.data());
            res->setChannel(i, out.data(), c);
        }
    }

    return res;
}