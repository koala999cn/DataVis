#include "KcHistoOp.h"
#include "KtSampling.h"
#include <vector>
#include "KcSampled1d.h"
#include <QPointF>


KcHistoOp::KcHistoOp(KvDataProvider* prov)
    : KvDataOperator("histo", prov)
{
    auto xrange = prov->range(0);
    xmin_ = xrange.low(), xmax_ = xrange.high();

    bands_ = 9;
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
    prop.val = QPointF(xmin_, xmax_);
    KvPropertiedObject::KpProperty subProp;
    subProp.name = QStringLiteral("min");
    prop.children.push_back(subProp);
    subProp.name = QStringLiteral("max");
    prop.children.push_back(subProp);
    ps.push_back(prop);

    prop.id = kPrivate::k_bands;
    prop.name = QStringLiteral("bands");
    prop.val = bands_;
    prop.minVal = 1;
    prop.step = 1;
    auto objp = dynamic_cast<const KvDataProvider*>(parent());
    assert(objp != nullptr);
    if (objp->step(0) == KvData::k_unknown_step)
        prop.maxVal = 99;
    else
        prop.maxVal = std::floor((xmax_ - xmin_) / objp->step(0));  
    ps.push_back(prop);

    return ps;
}


void KcHistoOp::setPropertyImpl_(int id, const QVariant& newVal)
{
    switch (id) {
    case kPrivate::k_range:
        xmin_ = newVal.toPointF().x();
        xmax_ = newVal.toPointF().y();
        break;

    case kPrivate::k_bands:
        bands_ = newVal.toInt();
        break;
    };
}


kRange KcHistoOp::range(kIndex axis) const
{
    if (axis == 0) 
        return  { xmin_, xmax_ };

    return KvDataOperator::range(axis);
}


kReal KcHistoOp::step(kIndex axis) const
{
    if (axis == 0) {
        KtSampling<kReal> samp;
        samp.resetn(bands_, xmin_, xmax_, 0.5);
        return samp.dx();
    }

    return KvDataOperator::step(axis);
}


std::shared_ptr<KvData> KcHistoOp::processImpl_(std::shared_ptr<KvData> data)
{
    assert(xmin_ < xmax_);

    auto xrange = data->range(0);

    assert(bands_ > 0);

    KtSampling<kReal> samp;
    samp.resetn(bands_, xmin_, xmax_, 0.5);

    auto res = std::make_shared<KcSampled1d>(samp.dx(), 0.5);
    res->sampling().shiftLowTo(xmin_);
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
    }


    return res;
}