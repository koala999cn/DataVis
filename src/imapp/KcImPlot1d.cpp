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
        KvPlot::update();
        ImPlot::EndPlot();
    }
}