#include "KcImPlot1d.h"
#include "implot/implot.h"


KcImPlot1d::KcImPlot1d(const std::string_view& name)
    : KvImWindow(name)
    , KvPlot(nullptr)
{
    minSize_[0] = 240, minSize_[1] = 120;
}


void KcImPlot1d::updateImpl_()
{
    ImPlot::PushStyleColor(ImPlotCol_PlotBg, (const ImVec4&)background());

    if (ImPlot::BeginPlot("##")) {
        KvPlot::update();
        ImPlot::EndPlot();
    }

    ImPlot::PopStyleColor();
}