#include "KcRdBubble2d.h"
#include "KvData.h"
#include "prov/KvDataProvider.h"
#include "qcustomplot/qcustomplot.h"
#include "QCPBubbleChart.h"
#include "KtSampling.h"
#include "KvSampled.h"
#include <assert.h>
#include "KtuMath.h"
#include "QtAppEventHub.h"


namespace kPrivate
{
    enum KeBubble2dProperty
    {
        k_bubble2d_prop_id = 200, // 此前的id预留给KvRdCustomPlot

        k_gradient,
        k_interpolate,
        k_color_scale,
        k_zrange,
        k_key_size,
        k_value_size
    };
}

KcRdBubble2d::KcRdBubble2d(KvDataProvider* is)
    : KvRdCustomPlot(is, "bubble2d")
{
    assert(is->isDiscreted() && is->dim() <= 2);

    new QCPBubbleChart(customPlot_->xAxis, customPlot_->yAxis);

    //preRender_();


    // 设置map range
    auto xrange = is->range(0);
    auto yrange = is->range(1);
    auto zrange = is->range(2);
    customPlot_->xAxis->setRange({ xrange.low(), xrange.high() });
    customPlot_->yAxis->setRange({ yrange.low(), yrange.high() });
}


bool KcRdBubble2d::doRender_(std::shared_ptr<KvData> data)
{
    if (data == nullptr || data->size() == 0)
        return true;

    auto bubbleChart = dynamic_cast<QCPBubbleChart*>(customPlot_->plottable());
    assert(bubbleChart);

    auto prov = dynamic_cast<KvDataProvider*>(parent());

    assert(data->isDiscreted());
    auto disc = std::dynamic_pointer_cast<KvDiscreted>(data);

    auto& bubbleData = bubbleChart->data();
    bubbleData.setSize(disc->size());
    if (disc->dim() == 1) {
        for (kIndex i = 0; i < disc->size(); i++) {
            auto& pt = disc->pointAt(i, 0);
            bubbleData.setData(i, pt[0], pt[1], pt[1]);
        }
    }
    else {
        assert(disc->dim() == 2);       
        for (kIndex i = 0; i < disc->size(); i++) {
            auto& pt = disc->pointAt(i, 0);
            bubbleData.setData(i, pt[0], pt[1], pt[2]);
        }
    }
    bubbleData.recalculateRanges(); // TODO:

    customPlot_->replot(prov->isStream()
        ? QCustomPlot::rpQueuedRefresh : QCustomPlot::rpRefreshHint);

    return true;
}


void KcRdBubble2d::reset()
{
    auto bubbleChart = dynamic_cast<QCPBubbleChart*>(customPlot_->plottable());
    assert(bubbleChart);

    bubbleChart->data().clear();
}


KvPropertiedObject::kPropertySet KcRdBubble2d::propertySet() const
{
    kPropertySet ps = KvRdCustomPlot::propertySet();
    return ps;
}


void KcRdBubble2d::setPropertyImpl_(int id, const QVariant& newVal)
{
    KvRdCustomPlot::setPropertyImpl_(id, newVal);

    if (customPlot_->isVisible()) // 及时更新主题
        customPlot_->replot();
}


void KcRdBubble2d::preRender_()
{

}