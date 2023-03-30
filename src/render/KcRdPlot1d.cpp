#include "KcRdPlot1d.h"
#include "imapp/KcImPlot2d.h"
#include "plot/KcGraph.h"
#include "plot/KcScatter.h"
#include "plot/KcBars2d.h"
#include "plot/KcLineFilled.h"
#include "plot/KcAndrewsCurves.h"
#include "plot/KcBoxPlot.h"
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
	return 6;
}


int KcRdPlot1d::plottableType_(KvPlottable* plt) const
{
	if (dynamic_cast<KcAndrewsCurves*>(plt))
		return 4;
	else if (dynamic_cast<KcGraph*>(plt))
		return 0;
	else if (dynamic_cast<KcScatter*>(plt))
		return 1;
	else if (dynamic_cast<KcBars2d*>(plt))
		return 2;
	else if (dynamic_cast<KcLineFilled*>(plt))
		return 3;
	else if (dynamic_cast<KcBoxPlot*>(plt))
		return 5;

	return -1;
}


const char* KcRdPlot1d::plottableTypeStr_(int iType) const
{
	static const char* pltTypes[] = {
		"graph", "scatter", "bar", "area", "andrews curves", "box"
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
		return new KcAndrewsCurves(name);

	case 5:
		return new KcBoxPlot(name);
	}

	return nullptr;
}


bool KcRdPlot1d::plottableMatchData_(int iType, const KvData& d) const
{
	switch (iType)
	{
	case 4:
		if (d.isContinued()) // KcAndrewsCurves不支持连续数据
			return false;
		break;

	default:
		break;
	}

	return super_::plottableMatchData_(iType, d);
}


namespace kPrivate
{
	void showPlottableSpecificProperty1d(KvPlottable* plt)
	{
		auto plt1d = dynamic_cast<KvPlottable1d*>(plt);
		if (plt1d && !plt1d->empty()) {

			int dim[3] = { int(plt1d->xdim() + 1), int(plt1d->ydim() + 1), int(plt1d->zdim() + 1) };
			if (ImGui::SliderInt3("Dim Mapping", dim, 1, plt1d->odata()->dim() + 1)) {
				dim[0] = KuMath::clamp<int>(dim[0] - 1, 0, plt1d->odata()->dim());
				plt1d->setXdim(dim[0]);

				dim[1] = KuMath::clamp<int>(dim[1] - 1, 0, plt1d->odata()->dim());
				plt1d->setYdim(dim[1]);

				dim[2] = KuMath::clamp<int>(dim[2] - 1, 0, plt1d->odata()->dim());
				plt1d->setZdim(dim[2]);
			}
		}

		if (dynamic_cast<KcLineFilled*>(plt)) {
			auto fill = dynamic_cast<KcLineFilled*>(plt);

			static const char* modeStr[] = {
				"overlay", "between", "delta"
			};
			int fillMode = fill->fillMode();
			if (ImGui::Combo("Fill Mode", &fillMode, modeStr, std::size(modeStr))) {
				fill->setFillMode(KcLineFilled::KeFillMode(fillMode));
			}

			ImGui::BeginDisabled(fillMode != 0);
			static const char* baseStr[] = {
				"x line", "y line", "point"
			};
			int baseMode = fill->baseMode();
			if (ImGui::Combo("Base Mode", &baseMode, baseStr, std::size(baseStr))) {
				fill->setBaseMode(KcLineFilled::KeBaseMode(baseMode));
			}

			if (baseMode < 2) {
				float line = fill->baseLine();
				if (ImGui::DragFloat("Base Line", &line, line ? line / 1000 : 1, -1e6, 1e6, 
					baseMode == 0 ? "y = %.3f" : "x = %.3f"))
					fill->setBaseLine(line);
			}
			else {
				auto pt = fill->basePoint(); 
				if (ImGui::DragScalarN("Base Point", ImGuiDataType_Double, pt.data(), 3))
					fill->setBasePoint(pt);
			}

			ImGui::EndDisabled();

			//ImGuiX::brush(fill->fillBrush(), false); // 隐藏brush的style选项，始终fill

			bool open(false);
			ImGuiX::cbTreePush("Line", &fill->showLine(), &open);
			if (open) {
				ImGuiX::pen(fill->linePen(), true, true);
				ImGuiX::cbTreePop();
			}
		}
		else if (dynamic_cast<KcScatter*>(plt)) {
			auto scat = dynamic_cast<KcScatter*>(plt);
			if (ImGuiX::treePush("Marker", false)) {
				auto m = scat->marker();
				if (ImGuiX::marker(m))
					scat->setMarker(m);
				ImGuiX::treePop();
			}

			ImGui::BeginDisabled(scat->marker().type == KpMarker::k_dot);

			bool open(false);
			bool varyingSize = scat->sizeVarying();
			if (ImGuiX::cbTreePush("Size Varying", &varyingSize, &open))
				scat->setSizeVarying(varyingSize);
			
			if (open) {
				int dim = scat->sizeVaryingDim();
				if (ImGui::SliderInt("Varying Dim", &dim, 0, scat->odim())) {
					dim = KuMath::clamp<int>(dim, 0, scat->odim());
					scat->setSizeVaryingDim(dim);
				}

				float lower = scat->sizeLower();
				float upper = scat->sizeUpper();
				if (ImGui::DragFloatRange2("Varying Range", &lower, &upper, 0.1, 3, 33, "%.1f")) {
					scat->setSizeLower(lower);
					scat->setSizeUpper(upper);
				}
					
				bool varyingByArea = scat->sizeVaryingByArea();
				if (ImGui::Checkbox("Varying by Area", &varyingByArea))
					scat->setSizeVaryingByArea(varyingByArea);
				ImGuiX::cbTreePop();
			}

			ImGui::EndDisabled();

			ImGuiX::cbTreePush("Labeling", &scat->showLabel(), &open);
			if (open) {
				auto label = scat->label();
				if (ImGuiX::label(label))
					scat->setLabel(label);

				int d = scat->labelingDim();
				if (ImGui::SliderInt("Dim", &d, 0, scat->odim())) {
					d = KuMath::clamp<int>(d, 0, scat->odim());
					scat->setLabelingDim(d);
				}

				ImGuiX::cbTreePop();
			}
		}
		else if (dynamic_cast<KcGraph*>(plt)) {
			auto graph = dynamic_cast<KcGraph*>(plt);
			if (ImGuiX::treePush("Line", false)) {
				ImGuiX::pen(graph->linePen(), true, false);
				ImGuiX::treePop();
			}
		}
		else if (dynamic_cast<KcBars2d*>(plt)) {
			auto bars = dynamic_cast<KcBars2d*>(plt);

			if (ImGuiX::treePush("Layout", false)) {

				auto baseLine = bars->baseLine();
				if (ImGui::DragFloat("Baseline", &baseLine))
					bars->setBaseLine(baseLine);

				auto widthRatio = bars->barWidthRatio();
				if (ImGui::SliderFloat("Width Ratio", &widthRatio, 0.01, 1.0, "%.2f"))
					bars->setBarWidthRatio(widthRatio);

				auto padding = bars->stackPadding();
				if (ImGui::SliderFloat("Stack Padding", &padding, 0.0, 49.0, "%.1f px") && padding >= 0)
					bars->setStackPadding(padding);

				ImGuiX::treePop();
			}

			ImGui::Checkbox("Fill", &bars->showFill());

			bool open = false;
			ImGuiX::cbTreePush("Border", &bars->showBorder(), &open);
			if (open) {
				ImGuiX::pen(bars->borderPen(), true, true);
				ImGuiX::cbTreePop();
			}
		}
		else if (dynamic_cast<KcBoxPlot*>(plt)) {
			auto box = dynamic_cast<KcBoxPlot*>(plt);

			if (ImGuiX::treePush("Center Box", false)) {

				ImGui::DragFloat("Width", &box->boxWidth(), 0.01, 0, 1, "%.2f");

				if (ImGuiX::treePush("Median", false)) {
					ImGuiX::pen(box->medianPen(), true, true);
					ImGuiX::cbTreePop();
				}

				if (ImGuiX::treePush("Border", false)) {
					ImGuiX::pen(box->borderPen(), true, true); // 颜色值与辅色一致
					ImGuiX::cbTreePop();
				}

				ImGuiX::cbTreePop();
			}

			if (ImGuiX::treePush("Whisker", false)) {

				ImGui::DragFloat("Length Factor", &box->whisLengthFactor());

				if (ImGuiX::treePush("Line", false)) {
					ImGuiX::pen(box->whisPen(), true, true);
					ImGuiX::treePop();
				}

				if (ImGuiX::treePush("Endding Bar", false)) {
					ImGui::DragFloat("Width", &box->whisBarWidth(), 0.01, 0, 1, "%.2f");
					ImGuiX::pen(box->whisBarPen(), true, true);
					ImGuiX::treePop();
				}

				ImGuiX::treePop();
			}

			if (ImGuiX::treePush("Outlier", false)) {
				ImGuiX::marker(box->outlierMarker());
				ImGuiX::treePop();
			}
		}
	}
}

void KcRdPlot1d::showPlottableSpecificProperty_(KvPlottable* plt)
{
	kPrivate::showPlottableSpecificProperty1d(plt);
}
