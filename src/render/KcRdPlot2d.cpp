#include "KcRdPlot2d.h"
#include "imapp/KcImPlot2d.h"
#include "plot/KcHeatMap.h"
#include "prov/KvDataProvider.h"
#include "imguix.h"


KcRdPlot2d::KcRdPlot2d()
    : super_("Plot2d", std::make_shared<KcImPlot2d>("Plot2d"))
{

}


std::vector<KvPlottable*> KcRdPlot2d::createPlottable_(KcPortNode* port)
{
    return createPlts_<KcHeatMap>(port);
}


unsigned KcRdPlot2d::supportPlottableTypes_() const
{
    return 1;
}


int KcRdPlot2d::plottableType_(KvPlottable* plt) const
{
    if (dynamic_cast<KcHeatMap*>(plt))
        return 0;

    return -1;
}


const char* KcRdPlot2d::plottableTypeStr_(int iType) const
{
    static const char* pltTypes[] = {
        "Heat Map"
    };

    return pltTypes[iType];
}


KvPlottable* KcRdPlot2d::newPlottable_(int iType, const std::string& name)
{
    switch (iType)
    {
    case 0:
        return new KcHeatMap(name);
    }

    return nullptr;
}


void KcRdPlot2d::onInput(KcPortNode* outPort, unsigned inPort)
{
    super_::onInput(outPort, inPort);
    if (plot_->autoFit()) 
        for (unsigned i = 0; i < plot_->plottableCount(); i++)
            plot_->plottableAt(i)->fitColorMappingRange();
}


bool KcRdPlot2d::permitInput(int dataSpec, unsigned inPort) const
{
    assert(inPort == 0);

    KpDataSpec sp(dataSpec);
    return sp.dim == 2 && 
        (sp.type == k_sampled || sp.type == k_continued || sp.type == k_array);
}


namespace kPrivate
{
    void showPlottableSpecificProperty2d(KvPlottable* plt)
    {
        auto cmap = dynamic_cast<KvPlottable2d*>(plt);
        assert(cmap);

        bool open(false);
        ImGuiX::cbTreePush("Edge", &cmap->showBorder(), &open);
        if (open) {
            ImGuiX::pen(cmap->borderPen(), true);
            ImGuiX::cbTreePop();
        }

        open = false;
        //ImGuiX::cbTreePush("Text", &cmap->showText(), &open);
        //if (open) {
        //    ImGui::ColorEdit4("Text Color", cmap->textColor());
        //    ImGui::ShowFontSelector("Font");
        //    ImGuiX::cbTreePop();
        //}
    }
}

void KcRdPlot2d::showPlottableSpecificProperty_(unsigned idx)
{
    auto plt = plot_->plottableAt(idx);
    kPrivate::showPlottableSpecificProperty2d(plt);
}
