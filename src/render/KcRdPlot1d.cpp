#include "KcRdPlot1d.h"
#include "imapp/KcImPlot2d.h"
#include "plot/KcGraph3d.h"
#include "prov/KvDataProvider.h"
#include "KuStrUtil.h"


KcRdPlot1d::KcRdPlot1d()
	: super_("Plot1d", std::make_shared<KcImPlot2d>("Plot1d"))
{

}


std::vector<KvPlottable*> KcRdPlot1d::createPlottable_(KvDataProvider* prov)
{
	if (prov->channels() == 1) 
		return { new KcGraph3d(prov->name()) };

	std::vector<KvPlottable*> plts;
	plts.resize(prov->channels());

	for (kIndex ch = 0; ch < prov->channels(); ch++) {
		std::string name = prov->name() + " - ch" + KuStrUtil::toString(ch);
		plts[ch] = new KcGraph3d(name);
	}

	return plts;
}
