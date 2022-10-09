#include "KcImPlot1d.h"
#include "implot/implot.h"


KcImPlot1d::KcImPlot1d(const std::string_view& name)
    : KvImWindow(name)
    , KvPlot(nullptr)
{

}


void KcImPlot1d::updateImpl_()
{
    if (ImPlot::BeginPlot("##")) {
        //ImPlot::PlotBars("My Bar Plot", bar_data, 11);
        //ImPlot::PlotLine("My Line Plot", x_data, y_data, 1000);
        ImPlot::EndPlot();
    }
}