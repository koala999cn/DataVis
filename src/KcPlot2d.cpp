#include "KcPlot2d.h"
#include "KvData2d.h"
#include "KvDataProvider.h"
#include "qcustomplot/qcustomplot.h"
#include <assert.h>


KcPlot2d::KcPlot2d(KvDataProvider* is)
    : KvCustomPlot(is, "color_map")
{
    auto colorMap = new QCPColorMap(customPlot_->xAxis, customPlot_->yAxis);
    auto data = colorMap->data();
}


bool KcPlot2d::render(std::shared_ptr<KvData> data)
{
    auto colorMap = dynamic_cast<QCPColorMap*>(customPlot_->plottable());
    auto data2d = std::dynamic_pointer_cast<KvData2d>(data);
    assert(colorMap && data2d);

    colorMap->data()->setSize(data2d->length(0), data2d->length(1));
    auto xRange = data2d->range(0); auto yRange = data2d->range(1);
    colorMap->data()->setRange(QCPRange(xRange.low(), xRange.high()), 
                                QCPRange(yRange.low(), yRange.high()));
    
    for (int x = 0; x < data2d->length(0); ++x)
        for (int y = 0; y < data2d->length(1); ++y)
            colorMap->data()->setCell(x, y, data2d->value(x, y).z);

    auto zRange = data2d->range(2);
    colorMap->setDataRange(QCPRange(zRange.low(), zRange.high()));
    customPlot_->rescaleAxes();
    customPlot_->replot();

    return true;
}


void KcPlot2d::reset()
{
    auto colorMap = dynamic_cast<QCPColorMap*>(customPlot_->plottable());
    assert(colorMap);

    colorMap->data()->clear();
    colorMap->data()->clearAlpha();
}