#include "KcRdPlot1d.h"
#include "imapp/KcImPlot2d.h"
#include "plot/KcGraph.h"
#include "plot/KcScatter.h"
#include "plot/KcBars2d.h"
#include "prov/KvDataProvider.h"
#include "KuStrUtil.h"
#include "imgui.h"


KcRdPlot1d::KcRdPlot1d()
	: super_("Plot1d", std::make_shared<KcImPlot2d>("Plot1d"))
{

}


std::vector<KvPlottable*> KcRdPlot1d::createPlottable_(KvDataProvider* prov)
{
	// 根据prov自动选择图类型
	if (prov->isScattered())
		return createPlts_<KcScatter>(prov);
	else if(prov->isSeries())
		return createPlts_<KcBars2d>(prov);
	else // if (prov->isContinued() || prov->isSampled())
		return createPlts_<KcGraph>(prov);
}


int KcRdPlot1d::plottableType_(KvPlottable* plt)
{
	if (dynamic_cast<KcGraph*>(plt))
		return 0;
	else if (dynamic_cast<KcScatter*>(plt))
		return 1;
	else if (dynamic_cast<KcBars2d*>(plt))
		return 2;

	return -1;
}


KvPlottable* KcRdPlot1d::createPlottable_(int type, const std::string& name)
{
	switch (type)
	{
	case 0:
		return new KcGraph(name);

	case 1:
		return new KcScatter(name);

	case 2:
		return new KcBars2d(name);
	}

	return nullptr;
}


void KcRdPlot1d::showPlottableProperty_(unsigned idx)
{
    static const char* pltTypes[] = {
		"graph", "scatter", "bar"
    };

	int type = plottableType_(plot_->plottableAt(idx));

    if (ImGui::BeginCombo("Type", type >= 0 ? pltTypes[type] : nullptr)) {
        for (int i = 0; i < std::size(pltTypes); i++)
			if (ImGui::Selectable(pltTypes[i], i == type)) {
				auto oldPlt = plot_->plottableAt(idx);
				auto newPlt = createPlottable_(i, oldPlt->name());

				// clone the theme
				std::vector<color4f> majorColors(oldPlt->majorColors());
				for (unsigned c = 0; c < majorColors.size(); c++)
					majorColors[c]= oldPlt->majorColor(c);
				newPlt->setMajorColors(majorColors);
				newPlt->setMinorColor(oldPlt->minorColor());

				// clone the data
				newPlt->setData(oldPlt->data());

				// 同步port2Plts_
				for (auto& i : port2Plts_)
					if (i.second == oldPlt) {
						i.second = newPlt;  break;
					}

				plot_->setPlottableAt(idx, newPlt);
			}

        ImGui::EndCombo();
    }

}
