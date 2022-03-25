#include "KcPlot2d.h"
#include "KvData2d.h"
#include "KvDataProvider.h"
#include "qcustomplot/qcustomplot.h"
#include "../dsp/KtSampling.h"
#include <assert.h>



KcPlot2d::KcPlot2d(KvDataProvider* is)
    : KvCustomPlot(is, "color_map")
{
    auto colorMap = new QCPColorMap(customPlot_->xAxis, customPlot_->yAxis);
    auto data = colorMap->data();

    assert(is->dim() == 2);
    auto xrange = is->range(0); auto yrange = is->range(1);
    auto xstep = is->step(0); auto ystep = is->step(1);
    KtSampling<kReal> xsamp(xrange.low(), xrange.high(), xstep, xrange.low()), 
        ysamp(yrange.low(), yrange.high(), ystep, yrange.low());
    data->setSize(xsamp.count(), ysamp.count());
    data->setRange(QCPRange(xrange.low(),xrange.high()),
        QCPRange(yrange.low(), yrange.high()));
    dx_ = xstep;

    auto zrange = is->range(2);
    colorMap->setDataRange(QCPRange(zrange.low(), zrange.high()));

    colorMap->setGradient(QCPColorGradient::gpSpectrum);

    connect(customPlot_->xAxis, qOverload<const QCPRange&>(&QCPAxis::rangeChanged),
        [this, colorMap](const QCPRange& newRange) {
            if (colorMap->data()->keyRange() != newRange) {
                colorMap->data()->setKeyRange(newRange);

                KtSampling<kReal> xsamp(0, dx_, dx_, 0);
                colorMap->data()->setKeySize(xsamp.countLength(newRange.size()));
            }
        });
}


bool KcPlot2d::render(std::shared_ptr<KvData> data)
{
    auto colorMap = dynamic_cast<QCPColorMap*>(customPlot_->plottable());
    auto data2d = std::dynamic_pointer_cast<KvData2d>(data);
    assert(colorMap && data2d);

    auto mapData = colorMap->data();
    auto prov = dynamic_cast<KvDataProvider*>(parent());
    if (prov->isStream()) {
        
        if (mapData->valueSize() != data->length(1)) {
            mapData->setValueSize(data->length(1));
            auto r = prov->range(1);
            mapData->setValueRange({ r.low(), r.high() }); 
            customPlot_->yAxis->setRange({ r.low(), r.high() });
        }

        if (dx_ != data->step(0)) { // framing的shift值可能动态改变
            dx_ = data->step(0);
            KtSampling<kReal> xsamp(0, dx_, dx_, 0);
            mapData->setKeySize(xsamp.countLength(mapData->keyRange().size()));
        }


        // TODO: 假定data均匀采样
        //assert(data2d->range(1) == prov->range(1));
        assert(data2d->step(1) == prov->step(1));

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
    }
    else {
        mapData->setSize(data2d->length(0), data2d->length(1));

        for (int x = 0; x < data2d->length(0); ++x)
            for (int y = 0; y < data2d->length(1); ++y)
                mapData->setCell(x, y, data2d->value(x, y).z);
      
        //if (autoScale_) {
       //     customPlot_->rescaleAxes();
       //     colorMap->rescaleDataRange();
       // }
    }
    
    show(true);

    customPlot_->replot(prov->isStream()
        ? QCustomPlot::rpQueuedRefresh : QCustomPlot::rpRefreshHint);

    return true;
}


void KcPlot2d::reset()
{
    auto colorMap = dynamic_cast<QCPColorMap*>(customPlot_->plottable());
    assert(colorMap);

    colorMap->data()->clear();
    colorMap->data()->clearAlpha();
}


namespace kPrivate
{
    enum KePlot2dProperty
    {
        k_plot2d_prop_id = 200, // 此前的id预留给KvCustomPlot

        k_gradient,
        k_interpolate,
        k_zrange
    };
}


KvPropertiedObject::kPropertySet KcPlot2d::propertySet() const
{
    using namespace kPrivate;

    kPropertySet ps = KvCustomPlot::propertySet();

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


    ps.push_back(prop);

    return ps;
}


void KcPlot2d::onPropertyChanged(int id, const QVariant& newVal)
{
    using namespace kPrivate;

    assert(id >= 0);

    if (id <= k_plot2d_prop_id) {
        KvCustomPlot::onPropertyChanged(id, newVal);
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

        case k_zrange:
            colorMap->setDataRange(QCPRange(newVal.toPointF().x(), newVal.toPointF().y()));
            break;
        }
    }

    if (customPlot_->isVisible())
        customPlot_->replot();
}
