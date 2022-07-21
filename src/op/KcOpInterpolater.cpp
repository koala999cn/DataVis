#include "KcOpInterpolater.h"
#include "dsp/KcInterpolater.h"
#include <QPointF>


KcOpInterpolater::KcOpInterpolater(KvDataProvider* prov)
    : KvDataOperator("interpolate", prov)
{
    preRender_();
}


kReal KcOpInterpolater::step(kIndex axis) const
{
    assert(axis < dim());
    return 0;
}


kIndex KcOpInterpolater::size(kIndex axis) const
{
    assert(axis < dim());
    return KvData::k_inf_size;
}



namespace kPrivate
{
    enum KeInterpolaterProperty
    {
        k_dim,
        k_channels,
        k_range_x,
        k_range_y,
        k_range_z,
        k_interp,
        k_extrap
    };
}


KcOpInterpolater::kPropertySet KcOpInterpolater::propertySet() const
{
    using namespace kPrivate;

    kPropertySet ps;

    KpProperty prop;

    prop.id = k_dim;
    prop.name = tr("Dim");
    prop.flag = k_readonly;
    prop.val = dim();
    ps.push_back(prop);

    prop.id = k_channels;
    prop.name = tr("Channels");
    prop.flag = k_readonly;
    prop.val = channels();
    ps.push_back(prop);

    prop.id = k_range_x;
    prop.name = QStringLiteral("Range");
    prop.flag = KvPropertiedObject::k_readonly;
    prop.val = QPointF(range(0).low(), range(0).high());
    KvPropertiedObject::KpProperty subProp;
    subProp.name = QStringLiteral("low");
    prop.children.push_back(subProp);
    subProp.name = QStringLiteral("high");
    prop.children.push_back(subProp);
    ps.push_back(prop);


    static const std::pair<QString, int> interp[] = {
        { "Linear", KcInterpolater::k_linear },
        { "Quad", KcInterpolater::k_quad }
    };
    prop.id = kPrivate::k_interp;
    prop.name = u8"InterpolateMethod";
    prop.val = interpMethod_;
    prop.flag = 0;
    prop.children.clear();
    prop.makeEnum(interp);
    ps.push_back(prop);


    static const std::pair<QString, int> extrap[] = {
        { "Nan", KcInterpolater::k_nan },
        { "Zero", KcInterpolater::k_zero },
        { "Const", KcInterpolater::k_const },
        { "Mirro", KcInterpolater::k_mirro },
        { "Period", KcInterpolater::k_period },
        { "Extra", KcInterpolater::k_extra },

    };
    prop.id = kPrivate::k_extrap;
    prop.name = u8"ExtrapolateMethod";
    prop.val = extrapMethod_;
    prop.makeEnum(extrap);
    ps.push_back(prop);


    return ps;
}



void KcOpInterpolater::setPropertyImpl_(int id, const QVariant& newVal)
{
    switch (id) {
    case kPrivate::k_interp:
        interpMethod_ = newVal.toInt();
        break;

    case kPrivate::k_extrap:
        extrapMethod_ = newVal.toInt();
        break;
    }
}


void KcOpInterpolater::preRender_()
{
    auto objp = dynamic_cast<KvDataProvider*>(parent());
    assert(objp);
}


std::shared_ptr<KvData> KcOpInterpolater::processImpl_(std::shared_ptr<KvData> data)
{
    assert(data->isDiscreted());

    auto dis = std::dynamic_pointer_cast<KvDiscreted>(data);
    auto interp = std::make_shared<KcInterpolater>(dis);
    interp->setInterMethod(interpMethod_);
    interp->setExtraMethod(extrapMethod_);
    return interp;
}
