#include "KcRdPlot1d.h"
#include "imapp/KcImPlot1d.h"
#include "imapp/KcImGraph2d.h"
#include "prov/KvDataProvider.h"
#include "KuStrUtil.h"


KcRdPlot1d::KcRdPlot1d()
	: super_("Plot1d", std::make_shared<KcImPlot1d>("Plot1d"))
{

}


std::vector<KvPlottable*> KcRdPlot1d::createPlottable_(KvDataProvider* prov)
{
	if (prov->channels() == 1) 
		return { new KcImGraph2d(prov->name()) };

	std::vector<KvPlottable*> plts;
	plts.resize(prov->channels());

	for (kIndex ch = 0; ch < prov->channels(); ch++) {
		std::string name = prov->name() + " - ch" + KuStrUtil::toString(ch);
		plts[ch] = new KcImGraph2d(name);
	}

	return plts;
}
