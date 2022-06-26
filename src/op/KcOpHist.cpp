#include "KcOpHist.h"
#include "KtSampling.h"
#include <vector>
#include "KcSampled1d.h"
#include <QPointF>
#include "KgHist.h"


KcOpHist::KcOpHist(KvDataProvider* prov)
    : KvDataOperator("hist", prov)
{
    assert(prov->dim() == 1);
    auto xrange = prov->range(0);

    hist_ = std::make_unique<KgHist>();
    hist_->resetLinear(9, xrange.low(), xrange.high()); // 缺省9个bin
}


namespace kPrivate
{
    enum KeHistPropertyId
    {
        k_range,
        k_bands
    };
};

KcOpHist::kPropertySet KcOpHist::propertySet() const
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
    assert(objp != nullptr && objp->step(0) != 0);
    prop.maxVal = std::floor((r.second - r.first) / objp->step(0));  
    ps.push_back(prop);

    return ps;
}


void KcOpHist::setPropertyImpl_(int id, const QVariant& newVal)
{
    switch (id) {
    case kPrivate::k_range:
        hist_->resetLinear(hist_->numBins(), newVal.toPointF().x(), newVal.toPointF().y());
        break;

    case kPrivate::k_bands:
        hist_->resetLinear(newVal.toInt(), hist_->range().first, hist_->range().second);
        break;
    };
}


kRange KcOpHist::range(kIndex axis) const
{
    if (axis == 0) 
        return { hist_->range().first, hist_->range().second };

    return KvDataOperator::range(axis);
}


kReal KcOpHist::step(kIndex axis) const
{
    if (axis == 0) {
        return hist_->binWidth(0); // bin是线性的
    }

    return KvDataOperator::step(axis);
}


kIndex KcOpHist::size(kIndex axis) const
{
    if (axis == 0) 
        return hist_->numBins();

    return KvDataOperator::step(axis);
}

void KcOpHist::preRender_()
{

}


std::shared_ptr<KvData> KcOpHist::processImpl_(std::shared_ptr<KvData> data)
{
    auto res = std::make_shared<KcSampled1d>();
    auto samp1d = std::dynamic_pointer_cast<KcSampled1d>(data);
    auto objp = dynamic_cast<KvDataProvider*>(parent());
    if (objp->isStream())
        samp1d->shiftLeftTo(0);
        
    hist_->process(*samp1d, *res);
    return res;
}
