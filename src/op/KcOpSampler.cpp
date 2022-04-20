#include "KcOpSampler.h"
#include "dsp/KtSampler.h"
#include <QPointF>


KcOpSampler::KcOpSampler(KvDataProvider* prov)
    : KvDataOperator("sampler", prov)
{
    syncParent();
}


kRange KcOpSampler::range(kIndex axis) const
{
    assert(axis <= dim());
    return axis < dim() ? samps_[axis] :
        dynamic_cast<KvDataProvider*>(parent())->range(axis); 
    // TODO: samps_的坐标范围可能超出parent的设定，此时得到的valueRange是不准确的
}


kReal KcOpSampler::step(kIndex axis) const
{
    assert(axis < dim());
    return samps_[axis].dx();
}


kIndex KcOpSampler::size(kIndex axis) const
{
    assert(axis < dim());
    return samps_[axis].count();
}



namespace kPrivate
{
    enum KeSamplerProperty
    {
        k_sampler_dim,
        k_sampler_channels,
        k_sampler_count,

        k_dim_length,
        k_dim_range,
        k_dim_step
    };

    static KvPropertiedObject::kPropertySet getDimProperties(kIndex dim, const KtSampling<kReal>& samp)
    {
        KvPropertiedObject::kPropertySet psAxis;
        KvPropertiedObject::KpProperty prop;

        int idBase = k_dim_length + dim * 3;
        prop.id = idBase + k_dim_length;
        prop.name = QStringLiteral("Length");
        prop.flag = KvPropertiedObject::k_readonly;
        prop.val = samp.count();
        psAxis.push_back(prop);

        prop.id = idBase + k_dim_step;
        prop.name = QStringLiteral("Step");
        prop.flag = 0;
        prop.val = samp.dx();
        psAxis.push_back(prop);

        prop.id = idBase + k_dim_range;
        prop.name = QStringLiteral("Range");
        prop.flag = KvPropertiedObject::k_restrict;
        prop.val = QPointF(samp.low(), samp.high());
        KvPropertiedObject::KpProperty subProp;
        subProp.name = QStringLiteral("low");
        prop.children.push_back(subProp);
        subProp.name = QStringLiteral("high");
        prop.children.push_back(subProp);
        psAxis.push_back(prop);

        return psAxis;
    }
}


KcOpSampler::kPropertySet KcOpSampler::propertySet() const
{
    using namespace kPrivate;

    kPropertySet ps;

    KpProperty prop;

    prop.id = k_sampler_dim;
    prop.name = tr("Dim");
    prop.flag = k_readonly;
    prop.val = dim();
    ps.push_back(prop);

    prop.id = k_sampler_channels;
    prop.name = tr("Channels");
    prop.flag = k_readonly;
    prop.val = channels();
    ps.push_back(prop);

    prop.id = k_sampler_count;
    prop.name = tr("Count");
    prop.flag = k_readonly;
    prop.val = count();
    ps.push_back(prop);

    for (kIndex i = 0; i < dim(); i++) {
        prop.id = KvPropertiedObject::kInvalidId;
        prop.name = QString("Dim %1").arg(i);
        prop.flag = 0;
        prop.val.clear();
        prop.children = getDimProperties(i, samps_[i]);
        ps.push_back(prop);
    }

    return ps;
}



void KcOpSampler::setPropertyImpl_(int id, const QVariant& newVal)
{

}


void KcOpSampler::syncParent()
{
    auto objp = dynamic_cast<KvDataProvider*>(parent());
    assert(objp);

    if (objp->dim() != samps_.size()) {
        samps_.resize(objp->dim());
        for (kIndex i = 0; i < objp->dim(); i++) {
            auto r = objp->range(i);
            auto low = r.low();
            auto high = r.high();
            if (std::isinf(low)) low = -1e8;
            if (std::isinf(high)) high = 1e8;
            samps_[i].resetn(1024, low, high, 0.5); // TODO: 1024改为定制值
        }
    }
}


std::shared_ptr<KvData> KcOpSampler::processImpl_(std::shared_ptr<KvData> data)
{
    assert(data->isContinued() && data->dim() == samps_.size());

    auto cond = std::dynamic_pointer_cast<KvContinued>(data);
    std::shared_ptr<KvSampled> res;
    switch (data->dim())
    {
    case 1:
        res = std::make_shared<KtSampler<1>>(cond);
        break;

    case 2:
        res = std::make_shared<KtSampler<2>>(cond);
        break;

    case 3:
        res = std::make_shared<KtSampler<3>>(cond);
        break;

    default:
        break;
    }

    if (res) {
        std::vector<kIndex> shape(data->dim());
        for (kIndex i = 0; i < data->dim(); i++) {
            shape[i] = samps_[i].count();
            res->reset(i, samps_[i].low(), samps_[i].dx(), 0.5);
        }

        res->resize(shape.data());
    }

    return res;
}
