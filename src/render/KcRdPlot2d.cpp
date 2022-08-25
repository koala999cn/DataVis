#include "KcRdPlot2d.h"
#include "KvData.h"
#include "prov/KvDataProvider.h"
#include "qcustomplot/qcustomplot.h"
#include "KtSampling.h"
#include "KvSampled.h"
#include "KtSampler.h"
#include "KvContinued.h"
#include <assert.h>
#include "KtuMath.h"
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

    if (is->isDiscreted())
        data->setKeySize(0); // 清零，以保证preRender_正确初始化
        
    //preRender_();

    // 添加colorScale
    colorScale_ = new QCPColorScale(customPlot_);
    customPlot_->plotLayout()->addElement(0, 1, colorScale_);
    colorMap->setColorScale(colorScale_);

    // 设置colorScale_与坐标轴的顶部和底部margin一致
    QCPMarginGroup* group = new QCPMarginGroup(customPlot_);
    colorScale_->setMarginGroup(QCP::msTop | QCP::msBottom, group);
    customPlot_->axisRect()->setMarginGroup(QCP::msTop | QCP::msBottom, group);

    // 设置map range
    auto xrange = is->range(0);
    auto yrange = is->range(1);
    auto zrange = is->range(2);
    colorMap->data()->setSize(is->size(0), is->size(1));
    colorMap->data()->setKeyRange(QCPRange(xrange.low(), xrange.high()));
    colorMap->data()->setValueRange(QCPRange(yrange.low(), yrange.high()));
    colorMap->setDataRange(QCPRange(zrange.low(), zrange.high()));

    // 设置默认的渐变器
    colorMap->setGradient(QCPColorGradient::gpSpectrum);


    // 当用户修改x轴范围时，同步调整keySize
    connect(customPlot_->xAxis, qOverload<const QCPRange&>(&QCPAxis::rangeChanged),
        [this, colorMap](const QCPRange& newRange) {
            colorMap->data()->setKeyRange(newRange);

            auto prov = dynamic_cast<KvDataProvider*>(parent());
            if (prov->isSampled()) {
                KtSampling<kReal> xsamp(0, 1, prov->step(0), 0);
                auto keySize = xsamp.size(newRange.size());
                colorMap->data()->setKeySize(keySize);
                emit kAppEventHub->objectPropertyChanged(this, kPrivate::k_key_size, int(keySize));
            }   
            else if (prov->isContinued() && contData_) {
                doRender_(contData_); 
            }
        });

    connect(customPlot_->yAxis, qOverload<const QCPRange&>(&QCPAxis::rangeChanged),
        [this, colorMap](const QCPRange& newRange) {
            colorMap->data()->setValueRange(newRange);
            auto prov = dynamic_cast<KvDataProvider*>(parent());
            if (prov->isContinued() && contData_) {
                doRender_(contData_); 
            }
        });
}


bool KcRdPlot2d::doRender_(std::shared_ptr<KvData> data)
{
    if (data == nullptr || data->size() == 0)
        return true;

    auto colorMap = dynamic_cast<QCPColorMap*>(customPlot_->plottable());
    assert(colorMap);

    auto mapData = colorMap->data();
    auto prov = dynamic_cast<KvDataProvider*>(parent());

    std::shared_ptr<KvSampled> samp;

    if (data->isContinued()) {
        contData_ = data;
        samp = std::make_shared<KtSampler<2>>(std::dynamic_pointer_cast<KvContinued>(data));
        auto rkey = mapData->keyRange();
        auto rval = mapData->valueRange();
        samp->reset(0, rkey.lower, rkey.size() / samp->size(0), 0.5);
        samp->reset(1, rval.lower, rval.size() / samp->size(1), 0.5);

        mapData->setSize(samp->size(0), samp->size(1));

        for (int x = 0; x < samp->size(0); ++x)
            for (int y = 0; y < samp->size(1); ++y)
               mapData->setCell(x, y, samp->value(x, y, 0));

        //if (autoScale_) {
        //     customPlot_->rescaleAxes();
        //     colorMap->rescaleDataRange();
        // }

    }
    else {
        samp = std::dynamic_pointer_cast<KvSampled>(data);

        int mapOffset(0), dataOffset(0);
        if (mapData->keySize() > samp->size(0)) { // 平移map数据
            mapOffset = mapData->keySize() - samp->size(0);
            for (int x = 0; x < mapOffset; x++)
                for (int y = 0; y < mapData->valueSize(); y++)
                    mapData->setCell(x, y, mapData->cell(mapData->keySize() - mapOffset + x, y));
        }
        else {
            dataOffset = samp->size(0) - mapData->keySize();
        }

        for (kIndex x = dataOffset; x < samp->size(0); x++)
            for (kIndex y = 0; y < std::min<int>(mapData->valueSize(), samp->size(1)); y++)
                mapData->setCell(mapOffset + x - dataOffset, y, samp->value(x, y, 0));
    }
   
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
    subProp.makeEnum(gradients);
    prop.children.push_back(subProp);


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


void KcRdPlot2d::preRender_()
{
 /*   auto prov = dynamic_cast<KvDataProvider*>(parent());
    if (prov->isContinued()) // TODO:
        return;

    auto colorMap = dynamic_cast<QCPColorMap*>(customPlot_->plottable());
    auto mapData = colorMap->data();

    if (mapData->keySize() == 0) { // 初始化
        assert(prov->dim() == 2);
        mapData->setKeySize(prov->size(0));
        auto xrange = prov->range(0);
        auto qrange = QCPRange(xrange.low(), xrange.high());
        mapData->setKeyRange(qrange);
        customPlot_->yAxis->setRange(qrange);
    }

    if (mapData->keySize() != prov->size(0)) { // framing的shift值可能动态改变
        mapData->setKeySize(prov->size(0));
        if (prov->size(0) == 0) { // 用户调大了输入数据的dx，导致dx > keyRange
            // 调整绘图参数，确保keySize等于1
            mapData->setKeySize(1);
            mapData->setKeyRange({ 0, prov->step(0) });
            customPlot_->xAxis->setRange({ 0, prov->step(0) });
        }
    }

    if (mapData->valueSize() != prov->size(1)) {
        mapData->setValueSize(prov->size(1));
        auto r = prov->range(1);
        mapData->setValueRange({ r.low(), r.high() });
        customPlot_->yAxis->setRange({ r.low(), r.high() });
    }*/
}