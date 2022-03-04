#include "KcHisto.h"
#include "KtSampling.h"
#include <vector>
#include "KcSampled1d.h"
#include "KtuMath.h"
#include <QPointF>


KcHisto::KcHisto(KvDataProvider* prov)
    : KvDataOperator("histo", prov)
{
    auto xrange = prov->range(0);
    xmin_ = xrange.first, xmax_ = xrange.second;

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

KcHisto::kPropertySet KcHisto::propertySet() const
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


void KcHisto::onPropertyChanged(int id, const QVariant& newVal)
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


kRange KcHisto::range(int axis) const
{
    if (axis == 0) 
        return  { xmin_, xmax_ };

    return KvDataOperator::range(axis);
}


kReal KcHisto::step(int axis) const
{
    if (axis == 0) {
        KtSampling<kReal> samp;
        samp.resetn(xmin_, xmax_, bands_, 0.5);
        return samp.dx();
    }

    return KvDataOperator::step(axis);
}


std::shared_ptr<KvData> KcHisto::processImpl_(std::shared_ptr<KvData> data)
{
    auto xrange = data->range(0);
    auto xmin = xmin_;
    auto xmax = KtuMath<kReal>::clamp(xmax_, xrange.first, xrange.second);

    assert(bands_ > 0);

    KtSampling<kReal> samp;
    samp.resetn(xmin, xmax, bands_, 0.5);

    auto res = std::make_shared<KcSampled1d>(samp.dx(), 0.5);
    res->sampling().shift(xmin);
    res->reserve(samp.nx());

    // skip points that out of left range
    auto data1d = std::dynamic_pointer_cast<KvData1d>(data);
    kIndex i = 0;
    while (i < data1d->count() && data1d->value(i).first < xmin)
        ++i;

    if (xmin < xmax) {
        auto limit = xmin + samp.dx();
        kReal sum(0);
        unsigned c(0);

        for (; i < data1d->count(); i++) {
            if (data1d->value(i).first < limit) { // accumulate current bar
                sum += data1d->value(i).second;
                ++c;
            }
            else { // goto next bar
                
                sum /= c;
                res->addSamples(&sum, 1);

                if (limit >= xmax)
                    break;

                limit += samp.dx();
                sum = 0;
                c = 0;
            }
        }

        if (c > 0) {
            sum /= c;
            res->addSamples(&sum, 1);
        }

    }

    return res;
}