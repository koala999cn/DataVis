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


bool KcRdPlot2d::permitInput(int dataSpec, unsigned inPort) const
{
    assert(inPort == 0);

    KpDataSpec sp(dataSpec);
    return sp.dim > 1 && 
        (sp.type == k_sampled || sp.type == k_continued || sp.type == k_array);
}


namespace kPrivate
{
    void showPlottableSpecificProperty1d(KvPlottable*);

    void showPlottableSpecificProperty2d(KvPlottable* plt)
    {
        showPlottableSpecificProperty1d(plt);

        auto plt2d = dynamic_cast<KvPlottable2d*>(plt);
        assert(plt2d);

        ImGui::Checkbox("Fill", &plt2d->showFill());

        bool open(false);
        ImGuiX::cbTreePush("Edge", &plt2d->showBorder(), &open);
        if (open) {
            ImGuiX::pen(plt2d->borderPen(), true, true);
            ImGuiX::cbTreePop();
        }

        auto heatmap = dynamic_cast<KcHeatMap*>(plt2d);
        if (heatmap) {
            open = false;
            ImGuiX::cbTreePush("Text", &heatmap->showText(), &open);
            if (open) {
                ImGui::ColorEdit4("Text Color", heatmap->textColor());
                ImGui::ShowFontSelector("Font");
                ImGuiX::cbTreePop();
            }
        }
    }
}


void KcRdPlot2d::showPlottableSpecificProperty_(KvPlottable* plt)
{
    kPrivate::showPlottableSpecificProperty2d(plt);
}
