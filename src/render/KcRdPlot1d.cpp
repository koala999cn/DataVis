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


unsigned KcRdPlot1d::supportPlottableTypes_() const
{
	return 3;
}


int KcRdPlot1d::plottableType_(KvPlottable* plt) const
{
	if (dynamic_cast<KcGraph*>(plt))
		return 0;
	else if (dynamic_cast<KcScatter*>(plt))
		return 1;
	else if (dynamic_cast<KcBars2d*>(plt))
		return 2;

	return -1;
}


const char* KcRdPlot1d::plottableTypeStr_(int iType) const
{
	static const char* pltTypes[] = {
		"graph", "scatter", "bar"
	};

	return pltTypes[iType];
}


KvPlottable* KcRdPlot1d::newPlottable_(int iType, const std::string& name)
{
	switch (iType)
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
