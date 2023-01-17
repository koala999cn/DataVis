#include "KcRdPlot1d.h"
#include "imapp/KcImPlot2d.h"
#include "plot/KcGraph.h"
#include "plot/KcScatter.h"
#include "plot/KcBars2d.h"
#include "plot/KcLineFilled.h"
#include "plot/KcBubble.h"
#include "prov/KvDataProvider.h"
#include "KuStrUtil.h"
#include "imguix.h"


KcRdPlot1d::KcRdPlot1d()
	: super_("Plot1d", std::make_shared<KcImPlot2d>("Plot1d"))
{

}


std::vector<KvPlottable*> KcRdPlot1d::createPlottable_(KcPortNode* port)
{
	auto prov = std::dynamic_pointer_cast<KvDataProvider>(port->parent().lock());
	// 根据prov自动选择图类型
	if (prov->isScattered(port->index()))
		return createPlts_<KcScatter>(port);
	else if(prov->dim(port->index()) > 1 || 
		prov->isSeries(port->index()) && prov->size(port->index(), 0) < 256)
		return createPlts_<KcBars2d>(port);
	else // if (prov->isContinued() || prov->isSampled())
		return createPlts_<KcGraph>(port);
}


unsigned KcRdPlot1d::supportPlottableTypes_() const
{
	return 5;
}


int KcRdPlot1d::plottableType_(KvPlottable* plt) const
{
	if (dynamic_cast<KcGraph*>(plt))
		return 0;
	else if (dynamic_cast<KcScatter*>(plt))
		return 1;
	else if (dynamic_cast<KcBars2d*>(plt))
		return 2;
	else if (dynamic_cast<KcLineFilled*>(plt))
		return 3;
	else if (dynamic_cast<KcBubble*>(plt))
		return 4;

	return -1;
}


const char* KcRdPlot1d::plottableTypeStr_(int iType) const
{
	static const char* pltTypes[] = {
		"graph", "scatter", "bar", "line-filled", "bubble"
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

	case 3:
		return new KcLineFilled(name);

	case 4:
		return new KcBubble(name);
	}

	return nullptr;
}


namespace kPrivate
{
	void showPlottableSpecificProperty1d(KvPlottable* plt)
	{
		if (dynamic_cast<KcLineFilled*>(plt)) {
			auto fill = dynamic_cast<KcLineFilled*>(plt);
			ImGuiX::brush(fill->fillBrush(), false); // 隐藏brush的style选项，始终fill
			if (ImGuiX::treePush("Line", false)) {
				ImGuiX::pen(fill->linePen(), true);
				ImGuiX::treePop();
			}
		}
		else if (dynamic_cast<KcScatter*>(plt)) {
			auto scat = dynamic_cast<KcScatter*>(plt);
			ImGuiX::marker(scat->marker());
		}
		else if (dynamic_cast<KcGraph*>(plt)) {
			auto graph = dynamic_cast<KcGraph*>(plt);
			ImGuiX::pen(graph->linePen(), true);
		}
		else if (dynamic_cast<KcBars2d*>(plt)) {
			auto bars = dynamic_cast<KcBars2d*>(plt);
			static const char* modes[] = {
				"stacked first",
				"grouped first"
			};
			int mode = bars->stackedFirst() ? 0 : 1;
			if (ImGuiX::combo("Mode", modes, mode))
				bars->stackedFirst() = (mode == 0);

			ImGui::DragFloat("Baseline", &bars->baseLine());
			ImGui::SliderFloat("Width Ratio", &bars->barWidthRatio(), 0.01, 1.0, "%.2f");
			ImGui::SliderFloat("Stack Padding", &bars->paddingStacked(), 0.0, 1.0, "%.2f");
			ImGui::SliderFloat("Group Padding", &bars->paddingGrouped(), 0.0, 1.0, "%.2f");

			bool open(false);
			ImGuiX::cbTreePush("Fill", &bars->showFill(), &open);
			if (open) {
				ImGuiX::brush(bars->fillBrush(), true);
				ImGuiX::treePop();
			}

			open = false;
			ImGuiX::cbTreePush("Border", &bars->showBorder(), &open);
			if (open) {
				ImGuiX::pen(bars->borderPen(), true);
				ImGuiX::treePop();
			}
		}
	}
}

void KcRdPlot1d::showPlottableSpecificProperty_(unsigned idx)
{
	auto plt = plot_->plottableAt(idx);
	kPrivate::showPlottableSpecificProperty1d(plt);
}
