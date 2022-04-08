#include "KcRdPlot2d.h"
#include "KvData2d.h"
#include "KvDataProvider.h"
#include "qcustomplot/qcustomplot.h"
#include "../dsp/KtSampling.h"
#include <assert.h>
#include "../base/KtuMath.h"
#include "QtAppEventHub.h"


namespace kPrivate
{
    enum KePlot2dProperty
    {
        k_plot2d_prop_id = 200, // 此前的id预留给KvRdCustomPlot

        k_gradient,
        k_interpolate,
        k_color_scale,
        k_zrange,
        k_key_size,
        k_value_size
    };
}

KcRdPlot2d::KcRdPlot2d(KvDataProvider* is)
    : KvRdCustomPlot(is, "color_map")
{
    auto colorMap = new QCPColorMap(customPlot_->xAxis, customPlot_->yAxis);
    auto data = colorMap->data();

    assert(is->dim() == 2);
    data->setKeySize(0); // 清零，以保证syncParentImpl_正确初始化
    syncParent();

    // 添加colorScale
    colorScale_ = new QCPColorScale(customPlot_);
    customPlot_->plotLayout()->addElement(0, 1, colorScale_);
    colorMap->setColorScale(colorScale_);

    // 设置colorScale_与坐标轴的顶部和底部margin一致
    QCPMarginGroup* group = new QCPMarginGroup(customPlot_);
    colorScale_->setMarginGroup(QCP::msTop | QCP::msBottom, group);
    customPlot_->axisRect()->setMarginGroup(QCP::msTop | QCP::msBottom, group);

    // data range需要手动初始化
    auto zrange = is->range(2);
    colorMap->setDataRange(QCPRange(zrange.low(), zrange.high()));

    // 设置默认的渐变器
    colorMap->setGradient(QCPColorGradient::gpSpectrum);


    // 当用户修改x轴范围时，需要响应调整keySize
    connect(customPlot_->xAxis, qOverload<const QCPRange&>(&QCPAxis::rangeChanged),
        [this, colorMap](const QCPRange& newRange) {
            if (colorMap->data()->keyRange() != newRange) {
                colorMap->data()->setKeyRange(newRange);

                KtSampling<kReal> xsamp(0, dx_, dx_, 0);
                auto keySize = xsamp.count(newRange.size());
                colorMap->data()->setKeySize(keySize);
                emit kAppEventHub->objectPropertyChanged(this, kPrivate::k_key_size, keySize);
            }
        });
}


bool KcRdPlot2d::renderImpl_(std::shared_ptr<KvData> data)
{
    if (data == nullptr || data->count() == 0)
        return true;

    auto colorMap = dynamic_cast<QCPColorMap*>(customPlot_->plottable());
    auto data2d = std::dynamic_pointer_cast<KvData2d>(data);
    assert(colorMap && data2d);

    auto mapData = colorMap->data();
    auto prov = dynamic_cast<KvDataProvider*>(parent());

    syncParent();

    //if (prov->isStream()) {

    assert(data2d->step(1) == prov->step(1));
    assert(data2d->length(1) == prov->length(1));

    int mapOffset(0), dataOffset(0);
    if (mapData->keySize() > data->length(0)) { // 平移map数据
        mapOffset = mapData->keySize() - data->length(0);
        for (int x = 0; x < mapOffset; x++)
            for (int y = 0; y < mapData->valueSize(); y++)
                mapData->setCell(x, y, mapData->cell(mapData->keySize() - mapOffset + x, y));
    }
    else {
        dataOffset = data->length(0) - mapData->keySize();
    }

    for (int x = dataOffset; x < data->length(0); x++)
        for (int y = 0; y < std::min<int>(mapData->valueSize(), data->length(1)); y++)
            mapData->setCell(mapOffset + x - dataOffset, y, data2d->value(x, y).z);
    //}
    //else {
    //    mapData->setSize(data2d->length(0), data2d->length(1));

    //    for (int x = 0; x < data2d->length(0); ++x)
    //        for (int y = 0; y < data2d->length(1); ++y)
    //            mapData->setCell(x, y, data2d->value(x, y).z);
      
        //if (autoScale_) {
       //     customPlot_->rescaleAxes();
       //     colorMap->rescaleDataRange();
       // }
   // }
    
    customPlot_->replot(prov->isStream()
        ? QCustomPlot::rpQueuedRefresh : QCustomPlot::rpRefreshHint);

    return true;
}


void KcRdPlot2d::reset()
{
    auto colorMap = dynamic_cast<QCPColorMap*>(customPlot_->plottable());
    assert(colorMap);

    colorMap->data()->clear();
    colorMap->data()->clearAlpha();
}


KvPropertiedObject::kPropertySet KcRdPlot2d::propertySet() const
{
    using namespace kPrivate;

    kPropertySet ps = KvRdCustomPlot::propertySet();

    KpProperty prop;
    prop.id = KvPropertiedObject::kInvalidId;
    prop.name = QStringLiteral("ColorMap");

    KpProperty subProp;

    auto colorMap = dynamic_cast<QCPColorMap*>(customPlot_->plottable());
    assert(colorMap);

    subProp.id = kPrivate::k_gradient;
    subProp.name = "Gradient";
    subProp.val = QVariant::fromValue<int>(QCPColorGradient::gpSpectrum);

    static const std::pair<QString, int> gradients[] = {
        { "Grayscale", QCPColorGradient::gpGrayscale },
        { "Hot", QCPColorGradient::gpHot },
        { "Cold", QCPColorGradient::gpCold },
        { "Night", QCPColorGradient::gpNight },
        { "Candy", QCPColorGradient::gpCandy },
        { "Geography", QCPColorGradient::gpGeography },
        { "Ion", QCPColorGradient::gpIon },
        { "Thermal", QCPColorGradient::gpThermal },
        { "Polar", QCPColorGradient::gpPolar },
        { "Spectrum", QCPColorGradient::gpSpectrum },
        { "Jet", QCPColorGradient::gpJet },
        { "Hues", QCPColorGradient::gpHues }
    };

    for (unsigned i = 0; i < sizeof(gradients) / sizeof(std::pair<QString, int>); i++) {
        KvPropertiedObject::KpProperty sub;
        sub.name = gradients[i].first;
        sub.val = gradients[i].second;
        subProp.children.push_back(sub);
    }
    prop.children.push_back(subProp);
    subProp.children.clear();


    subProp.id = kPrivate::k_interpolate;
    subProp.name = "Interpolate";
    subProp.val = colorMap->interpolate();
    prop.children.push_back(subProp);


    subProp.id = kPrivate::k_color_scale;
    subProp.name = "ColorScale";
    subProp.val = colorScale_->visible();
    prop.children.push_back(subProp);
    

    subProp.id = kPrivate::k_zrange;
    subProp.name = QStringLiteral("ZRange");
    subProp.desc = QStringLiteral("Data Range");
    subProp.flag = KvPropertiedObject::k_restrict;
    auto dr = colorMap->dataRange();
    subProp.val = QPointF(dr.lower, dr.upper);
    KvPropertiedObject::KpProperty subsubProp;
    subsubProp.name = QStringLiteral("low");
    subProp.children.push_back(subsubProp);
    subsubProp.name = QStringLiteral("high");
    subProp.children.push_back(subsubProp);
    prop.children.push_back(subProp);

    subProp.id = kPrivate::k_key_size;
    subProp.name = "KeySize";
    subProp.disp = tr("Key Size");
    subProp.desc.clear();
    subProp.val = colorMap->data()->keySize();
    subProp.flag = KvPropertiedObject::k_readonly;
    subProp.children.clear();
    prop.children.push_back(subProp);

    subProp.id = kPrivate::k_key_size;
    subProp.name = "ValueSize";
    subProp.disp = tr("Value Size");
    subProp.desc.clear();
    subProp.val = colorMap->data()->valueSize();
    subProp.flag = KvPropertiedObject::k_readonly;
    prop.children.push_back(subProp);

    ps.push_back(prop);

    return ps;
}


void KcRdPlot2d::setPropertyImpl_(int id, const QVariant& newVal)
{
    using namespace kPrivate;

    assert(id >= 0);

     if (id <= k_plot2d_prop_id) {
        KvRdCustomPlot::setPropertyImpl_(id, newVal);
    }
    else {
        auto colorMap = dynamic_cast<QCPColorMap*>(customPlot_->plottable());
        assert(colorMap);

        switch (id) {
        case k_gradient:
            colorMap->setGradient(QCPColorGradient::GradientPreset(newVal.toInt()));
            break;

        case k_interpolate:
            colorMap->setInterpolate(newVal.toBool());
            break;

        case k_color_scale:
            colorScale_->setVisible(newVal.toBool());
            if (newVal.toBool()) {
                customPlot_->plotLayout()->addElement(0, 1, colorScale_);
            }
            else {
                customPlot_->plotLayout()->take(colorScale_);
                customPlot_->plotLayout()->simplify();
            }
            break;

        case k_zrange:
            colorMap->setDataRange(QCPRange(newVal.toPointF().x(), newVal.toPointF().y()));
            break;
        }
    }

    if (customPlot_->isVisible())
        customPlot_->replot();
}


void KcRdPlot2d::syncParent()
{
    auto colorMap = dynamic_cast<QCPColorMap*>(customPlot_->plottable());
    auto mapData = colorMap->data();
    auto prov = dynamic_cast<KvDataProvider*>(parent());

    if (mapData->keySize() == 0) { // 初始化
        assert(prov->dim() == 2);
        dx_ = prov->step(0);
        mapData->setKeySize(prov->length(0));
        auto xrange = prov->range(0);
        auto qrange = QCPRange(xrange.low(), xrange.high());
        mapData->setKeyRange(qrange);
        customPlot_->yAxis->setRange(qrange);
    }

    if (dx_ != prov->step(0)) { // framing的shift值可能动态改变
        dx_ = prov->step(0);
        KtSampling<kReal> xsamp(0, dx_, dx_, 0);
        auto keySize = xsamp.count(mapData->keyRange().size());

        if (keySize == 0) { // 用户调大了输入数据的dx，导致dx > keyRange
            // 调整绘图参数，确保keySize等于1
            keySize = 1;
            mapData->setKeyRange({ 0, dx_ });
            customPlot_->xAxis->setRange({ 0, dx_ });
        }

        mapData->setKeySize(keySize);
    }

    if (mapData->valueSize() != prov->length(1)) {
        mapData->setValueSize(prov->length(1));
        auto r = prov->range(1);
        mapData->setValueRange({ r.low(), r.high() });
        customPlot_->yAxis->setRange({ r.low(), r.high() });
    }
}