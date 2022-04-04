﻿#include "KcHistOp.h"
#include "KtSampling.h"
#include <vector>
#include "KcSampled1d.h"
#include <QPointF>
#include "KgHist.h"


KcHistOp::KcHistOp(KvDataProvider* prov)
    : KvDataOperator("hist", prov)
{
    assert(prov->dim() == 1);
    auto xrange = prov->range(0);

    hist_ = std::make_unique<KgHist>();
    hist_->reset(9, xrange.low(), xrange.high()); // 缺省9个bin
}


namespace kPrivate
{
    enum KeHistPropertyId
    {
        k_range,
        k_bands
    };
};

KcHistOp::kPropertySet KcHistOp::propertySet() const
{
    kPropertySet ps;
    KpProperty prop;

    prop.id = kPrivate::k_range;
    prop.name = tr("Range");
    prop.flag = KvPropertiedObject::k_restrict;
    auto r = hist_->range();
    prop.val = QPointF(r.first, r.second);
    KvPropertiedObject::KpProperty subProp;
    subProp.name = tr("min");
    prop.children.push_back(subProp);
    subProp.name = tr("max");
    prop.children.push_back(subProp);
    ps.push_back(prop);

    prop.id = kPrivate::k_bands;
    prop.name = tr("Bands");
    prop.val = hist_->numBins();
    prop.minVal = 1;
    prop.step = 1;
    auto objp = dynamic_cast<const KvDataProvider*>(parent());
    assert(objp != nullptr);
    if (objp->step(0) == KvData::k_unknown_step)
        prop.maxVal = 99;
    else
        prop.maxVal = std::floor((r.second - r.first) / objp->step(0));  
    ps.push_back(prop);

    return ps;
}


void KcHistOp::setPropertyImpl_(int id, const QVariant& newVal)
{
    switch (id) {
    case kPrivate::k_range:
        hist_->reset(hist_->numBins(), newVal.toPointF().x(), newVal.toPointF().y());
        break;

    case kPrivate::k_bands:
        hist_->reset(newVal.toInt(), hist_->range().first, hist_->range().second);
        break;
    };
}


kRange KcHistOp::range(kIndex axis) const
{
    if (axis == 0) 
        return { hist_->range().first, hist_->range().second };

    return KvDataOperator::range(axis);
}


kReal KcHistOp::step(kIndex axis) const
{
    if (axis == 0) {
        return hist_->binWidth(0); // bin是线性的
    }

    return KvDataOperator::step(axis);
}


void KcHistOp::syncParent()
{

}


std::shared_ptr<KvData> KcHistOp::processImpl_(std::shared_ptr<KvData> data)
{
    auto res = std::make_shared<KcSampled1d>();
    hist_->process(*std::dynamic_pointer_cast<KcSampled1d>(data), *res);
    return res;
}