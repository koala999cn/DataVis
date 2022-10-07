#include "KcModuleImPlot.h"
#include "implot/implot.h"


KcModuleImPlot::KcModuleImPlot()
    : KvModule("ImPlot")
{

}


bool KcModuleImPlot::initialize()
{
    return ImPlot::CreateContext() != nullptr;
}


void KcModuleImPlot::deinitialize()
{
    ImPlot::DestroyContext();
}
