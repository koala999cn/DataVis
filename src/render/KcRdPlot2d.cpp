#include "KcRdPlot2d.h"
#include "imapp/KcImPlot2d.h"
#include "plot/KcColorMap.h"
#include "prov/KvDataProvider.h"


KcRdPlot2d::KcRdPlot2d()
    : super_("Plot2d", std::make_shared<KcImPlot2d>("Plot2d"))
{

}


std::vector<KvPlottable*> KcRdPlot2d::createPlottable_(KcPortNode* port)
{
    return createPlts_<KcColorMap>(port);
}


unsigned KcRdPlot2d::supportPlottableTypes_() const
{
    return 1;
}


int KcRdPlot2d::plottableType_(KvPlottable* plt) const
{
    if (dynamic_cast<KcColorMap*>(plt))
        return 0;

    return -1;
}


const char* KcRdPlot2d::plottableTypeStr_(int iType) const
{
    static const char* pltTypes[] = {
        "Color Map"
    };

    return pltTypes[iType];
}


KvPlottable* KcRdPlot2d::newPlottable_(int iType, const std::string& name)
{
    switch (iType)
    {
    case 0:
        return new KcColorMap(name);
    }

    return nullptr;
}


void KcRdPlot2d::onInput(KcPortNode* outPort, unsigned inPort)
{
    super_::onInput(outPort, inPort);
    if (plot_->autoFit()) {
        for (unsigned i = 0; i < plot_->plottableCount(); i++) {
            auto plt = dynamic_cast<KcColorMap*>(plot_->plottableAt(i));
            if (plt) {
                auto d = plt->data();
                auto r = d->valueRange();
                plt->valueLower() = r.low();
                plt->valueUpper() = r.high();
            }
        }
    }
}


bool KcRdPlot2d::permitInput(int dataSpec, unsigned inPort) const
{
    assert(inPort == 0);

    KpDataSpec sp(dataSpec);
    return sp.dim == 2 && (sp.type == k_sampled || sp.type == k_continued);
}
