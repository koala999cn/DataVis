#include "KcHistoOp.h"
#include "KtSampling.h"
#include <vector>
#include "KcSampled1d.h"
#include <QPointF>
#include "KgHist.h"


KcHistoOp::KcHistoOp(KvDataProvider* prov)
    : KvDataOperator("hist", prov)
{
    assert(prov->dim() == 1);
    auto xrange = prov->range(0);

    hist_ = std::make_unique<KgHist>();
    hist_->reset(9, xrange.low(), xrange.high()); // 缺省9个bin
}


namespace kPrivate
{
    enum KeHistoPropertyId
    {
        k_range,
        k_bands
    };
};

KcHistoOp::kPropertySet KcHistoOp::propertySet() const
{
    kPropertySet ps;
    KpProperty prop;

    prop.id = kPrivate::k_range;
    prop.name = QStringLiteral("range");
    prop.flag = KvPropertiedObject::k_restrict;
    auto r = hist_->range();
    prop.val = QPointF(r.first, r.second);
    KvPropertiedObject::KpProperty subProp;
    subProp.name = QStringLiteral("min");
    prop.children.push_back(subProp);
    subProp.name = QStringLiteral("max");
    prop.children.push_back(subProp);
    ps.push_back(prop);

    prop.id = kPrivate::k_bands;
    prop.name = QStringLiteral("bands");
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


void KcHistoOp::setPropertyImpl_(int id, const QVariant& newVal)
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


kRange KcHistoOp::range(kIndex axis) const
{
    if (axis == 0) 
        return { hist_->range().first, hist_->range().second };

    return KvDataOperator::range(axis);
}


kReal KcHistoOp::step(kIndex axis) const
{
    if (axis == 0) {
        return hist_->binWidth(0); // bin是线性的
    }

    return KvDataOperator::step(axis);
}


void KcHistoOp::syncParent()
{

}


std::shared_ptr<KvData> KcHistoOp::processImpl_(std::shared_ptr<KvData> data)
{
    auto res = std::make_shared<KcSampled1d>();
    hist_->process(*std::dynamic_pointer_cast<KcSampled1d>(data), *res);
    return res;

    /*assert(xmin_ < xmax_);

    auto xrange = data->range(0);

    assert(bands_ > 0);

    KtSampling<kReal> samp;
    samp.resetn(hist_->numBins(), hist_->range().first, hist_->range().second, 0.5);

    auto res = std::make_shared<KcSampled1d>(samp.dx(), 0.5);
    res->sampling().shiftLowTo(samp.x0());
    res->reserve(samp.count(), res->channels());

    // skip points that out of left range
    auto data1d = std::dynamic_pointer_cast<KvData1d>(data);


    // 跳过统计区间（左）之外的数据点
    kIndex i = 0;
    while (i < data1d->count() && data1d->value(i).x < xmin_)
        ++i;

    auto barRight = xmin_ + samp.dx();
    kReal sum(0);
    unsigned c(0);

    while (i < data1d->count()) {
        if (data1d->value(i).x < barRight) { // accumulate current bar
            sum += data1d->value(i++).y;
            ++c;
        }
        else { // goto next bar
             
            if (c > 0) {
                sum /= c;
                res->addSamples(&sum, 1);
            }

            if (barRight >= xmax_)
                break;

            barRight += samp.dx();
            sum = 0;
            c = 0;
        }
    }

    if (c > 0) {
        sum /= c;
        res->addSamples(&sum, 1);
    }*/
}