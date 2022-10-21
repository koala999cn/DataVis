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


void KcRdPlot1d::showProperySet() 
{
	super_::showProperySet();

	ImGui::Separator();


}
