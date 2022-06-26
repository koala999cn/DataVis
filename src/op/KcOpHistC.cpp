#include "KcOpHistC.h"
#include "KtSampling.h"
#include <vector>
#include "KcSampled1d.h"
#include <QPointF>
#include "KgHistC.h"
#include "KtuMath.h"


KcOpHistC::KcOpHistC(KvDataProvider* prov)
    : KvDataOperator("histC", prov)
{
    assert(prov->dim() == 1);
    auto yrange = prov->range(1);

    histc_ = std::make_unique<KgHistC>();
    histc_->resetLinear(9, yrange.low(), yrange.high()); // 缺省9个bin
}


namespace kPrivate
{
    enum KeHistPropertyId
    {
        k_range,
        k_bands
    };
};

KcOpHistC::kPropertySet KcOpHistC::propertySet() const
{
    kPropertySet ps;
    KpProperty prop;

    prop.id = kPrivate::k_range;
    prop.name = tr("Range");
    prop.flag = KvPropertiedObject::k_restrict;
    auto r = histc_->range();
    prop.val = QPointF(r.first, r.second);
    KvPropertiedObject::KpProperty subProp;
    subProp.name = tr("min");
    prop.children.push_back(subProp);
    subProp.name = tr("max");
    prop.children.push_back(subProp);
    ps.push_back(prop);

    prop.id = kPrivate::k_bands;
    prop.name = tr("Bands");
    prop.val = histc_->numBins();
    prop.minVal = 1;
    prop.step = 1;
    auto objp = dynamic_cast<const KvDataProvider*>(parent());
    assert(objp != nullptr && objp->step(0) != 0);
    prop.maxVal = std::floor((r.second - r.first) / objp->step(0));
    ps.push_back(prop);

    return ps;
}


void KcOpHistC::setPropertyImpl_(int id, const QVariant& newVal)
{
    switch (id) {
    case kPrivate::k_range:
        histc_->resetLinear(histc_->numBins(), newVal.toPointF().x(), newVal.toPointF().y());
        break;

    case kPrivate::k_bands:
        histc_->resetLinear(newVal.toInt(), histc_->range().first, histc_->range().second);
        break;
    };
}


kRange KcOpHistC::range(kIndex axis) const
{
    if (axis == 0)
        return { histc_->range().first, histc_->range().second };

    return { 0, 1 }; // 归一化
}


kReal KcOpHistC::step(kIndex axis) const
{
    if (axis == 0) {
        return histc_->binWidth(0); // bin是线性的
    }

    return KvDataOperator::step(axis);
}


void KcOpHistC::preRender_()
{

}


std::shared_ptr<KvData> KcOpHistC::processImpl_(std::shared_ptr<KvData> data)
{
    assert(data->channels() == 1); // TODO: 处理多通道

    auto samp1d = std::dynamic_pointer_cast<KcSampled1d>(data);
    histc_->count(samp1d->data(), samp1d->size());

    auto res = std::make_shared<KcSampled1d>();
    res->reset(0, histc_->range().first, histc_->binWidth(0), 0.5);
    res->resize(histc_->numBins(), 1);

    auto& cnt = histc_->result();
    std::vector<kReal> d(cnt.size());
    std::copy(cnt.cbegin(), cnt.cend(), d.begin());
    KtuMath<kReal>::scaleTo(d.data(), d.size(), 0.9);
    res->setChannel(nullptr, 0, d.data());
    return res;
}
