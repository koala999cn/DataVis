#include "KcRdPlot1d.h"
#include "imapp/KcImPlot2d.h"
#include "plot/KcGraph.h"
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
	if (prov->channels() == 1) 
		return { new KcBars2d(prov->name()) };

	std::vector<KvPlottable*> plts;
	plts.resize(prov->channels());

	for (kIndex ch = 0; ch < prov->channels(); ch++) {
		std::string name = prov->name() + " - ch" + KuStrUtil::toString(ch);
		plts[ch] = new KcBars2d(name);
	}

	return plts;
}


void KcRdPlot1d::showProperySet() 
{
	super_::showProperySet();

	ImGui::Separator();


}
