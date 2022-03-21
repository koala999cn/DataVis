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

    auto zrange = is->range(2);
    colorMap->setDataRange(QCPRange(zrange.low(), zrange.high()));
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
            qDebug("====%d vs %d", mapData->valueSize(), data->length(1));
        }

        // TODO: 假定data均匀采样

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
      
        if (autoScale_) {
            customPlot_->rescaleAxes();
            colorMap->rescaleDataRange();
        }
    }
    
    show(true);

    customPlot_->replot(prov->isStream()
        ? QCustomPlot::rpQueuedRefresh
        : QCustomPlot::rpRefreshHint);

    return true;
}


void KcPlot2d::reset()
{
    auto colorMap = dynamic_cast<QCPColorMap*>(customPlot_->plottable());
    assert(colorMap);

    colorMap->data()->clear();
    colorMap->data()->clearAlpha();
}