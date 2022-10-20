#include "KvRdPlot.h"
#include "plot/KvPlot.h"
#include "plot/KvPlottable.h"
#include "prov/KvDataProvider.h"
#include "dsp/KcDataMono.h"
#include "imapp/KvImWindow.h"
#include "imapp/KsImApp.h"
#include "imapp/KgImWindowManager.h"
#include "imgui.h"
#include "KuStrUtil.h"
#include "misc/cpp/imgui_stdlib.h"
#include "plot/KsThemeManager.h"
#include "plot/KcThemedPlotImpl_.h"
#include "plot/KvCoord.h"


KvRdPlot::KvRdPlot(const std::string_view& name, const std::shared_ptr<KvPlot>& plot)
	: KvDataRender(name)
	, plot_(plot)
{
	plot_->setVisible(false);

	auto imWindow = std::dynamic_pointer_cast<KvImWindow>(plot_);
	if (imWindow)
		KsImApp::singleton().windowManager().registerDynamic(imWindow);
}


KvRdPlot::~KvRdPlot()
{
	auto imWindow = std::dynamic_pointer_cast<KvImWindow>(plot_);
	if (imWindow)
		KsImApp::singleton().windowManager().releaseDynamic(imWindow);

	plot_ = nullptr;
}


void KvRdPlot::onInput(KcPortNode* outPort, unsigned inPort)
{
	assert(inPort == 0);

	auto pnode = outPort->parent().lock();
	assert(pnode);

	auto prov = std::dynamic_pointer_cast<KvDataProvider>(pnode);
	assert(prov);

	auto r = port2Plts_.equal_range(outPort->id());
	assert(r.first != r.second);

	auto data = prov->fetchData(outPort->index());
	auto numPlts = std::distance(r.first, r.second);
	
	if (data->channels() == 1 || numPlts == 1) {
		r.first->second->setData(data);
	}
	else {
		assert(data->channels() == numPlts);

		kIndex ch(0);
		auto disc = std::dynamic_pointer_cast<KvDiscreted>(data);
		assert(disc);

		for (auto i = r.first; i != r.second; i++) {
			assert(i->first == outPort->id());
			i->second->setData(std::make_shared<KcDataMono>(disc, ch++));
		}
	}
}


bool KvRdPlot::onNewLink(KcPortNode* from, KcPortNode* to)
{
	assert(to->parent().lock().get() == this && to->index() == 0);
	assert(port2Plts_.count(from->id()) == 0);

	auto pnode = from->parent().lock();
	assert(pnode);

	auto prov = std::dynamic_pointer_cast<KvDataProvider>(pnode);
	assert(prov);

	auto plts = createPlottable_(prov.get());
	if (plts.empty())
		return false;

	for (auto plt : plts) {
		plot_->addPlottable(plt);
		port2Plts_.insert(std::make_pair(from->id(), plt));
	}

	return true;
}


void KvRdPlot::onDelLink(KcPortNode* from, KcPortNode* to)
{
	assert(to->parent().lock().get() == this && to->index() == 0);

	auto pnode = from->parent().lock();
	assert(pnode);

	auto prov = std::dynamic_pointer_cast<KvDataProvider>(pnode);
	assert(prov);

	auto r = port2Plts_.equal_range(from->id());
	assert(r.first != r.second);

	for (auto i = r.first; i != r.second; i++)
		plot_->removePlottable(i->second);

	port2Plts_.erase(from->id());
}


bool KvRdPlot::onStartPipeline()
{
	plot_->setVisible(true);
	return true;
}


void KvRdPlot::showProperySet()
{
	bool vis = plot_->visible();
	if (ImGui::Checkbox("##Plot", &vis))
		plot_->setVisible(vis);

	ImGui::SameLine();
	super_::showProperySet(); // show the name property

	showThemeProperty_();

	ImGui::ColorEdit4("Background", plot_->background().color);

	ImGui::Checkbox("Auto Fit", &plot_->autoFit());

	if (plot_->plottableCount() > 0) {

		if (ImGui::TreeNodeEx("Plottable(s)", ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_CollapsingHeader)) {
			for (unsigned ch = 0; ch < plot_->plottableCount(); ch++) {
				ImGui::Indent();

				auto plt = plot_->plottable(ch);
				std::string label = "##Plottable" + KuStrUtil::toString(ch);
				ImGui::Checkbox(label.c_str(), &plt->visible());
				ImGui::SameLine();

				ImGui::PushID(plt);
				ImGui::InputText("##", &plt->name());
				
				for (unsigned i = 0; i < plt->majorColors(); i++) {
					ImGui::SameLine();
					std::string label = "##" + KuStrUtil::toString(i);
					ImGui::ColorEdit4(label.c_str(), plt->majorColor(i),
						ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
				}

				ImGui::PopID();

				ImGui::Unindent();
			}

			//ImGui::TreePop();
		}

	}

	auto lower = point3f(plot_->coord().lower());
	auto upper = point3f(plot_->coord().upper());
	auto speed = (upper - lower) * 0.1;
	for (unsigned i = 0; i < speed.size(); i++)
		if (speed.at(i) == 0)
			speed.at(i) = 1;

	bool extendsChanged(false);
	if (ImGui::DragFloatRange2("X-Axis", &lower.x(), &upper.x(), speed.x()))
		extendsChanged = true;
	if (ImGui::DragFloatRange2("Y-Axis", &lower.y(), &upper.y(), speed.y()))
		extendsChanged = true;
	if (ImGui::DragFloatRange2("Z-Axis", &lower.z(), &upper.z(), speed.z()))
		extendsChanged = true;

	if (extendsChanged) {
		plot_->coord().setExtents(lower, upper);
		plot_->autoFit() = false;
	}
}


void KvRdPlot::showThemeProperty_()
{
	KcThemedPlotImpl_ tp(*plot_);

	auto themes = KsThemeManager::singleton().listThemes();
	if (!themes.empty()) {

		auto iter = std::find(themes.cbegin(), themes.cend(), themeName_);
		//if (iter == themes.cend()) {
		//	themeName_ = themes.front();
		//	KsThemeManager::singleton().applyTheme(themeName_, &tp);
		//}

		
		if (ImGui::BeginCombo("Theme", themeName_.c_str(), ImGuiComboFlags_HeightLarge)) {
			for (iter = themes.cbegin(); iter != themes.cend(); iter++) {
				if (ImGui::Selectable(iter->c_str(), *iter == themeName_)) {
					themeName_ = *iter;
					applyTheme_(themeName_, &tp);
				}

				if (*iter == themeName_)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}
	}

	auto layouts = KsThemeManager::singleton().listLayouts();
	if (!layouts.empty()) {

		auto iter = std::find(layouts.cbegin(), layouts.cend(), layoutName_);
		//if (iter == layouts.cend()) {
		//	layoutName_ = layouts.front();
		//	KsThemeManager::singleton().applyTheme(layoutName_, &tp);
		//}

		if (ImGui::BeginCombo("Layout", layoutName_.c_str(), ImGuiComboFlags_HeightLarge)) {
			for (iter = layouts.cbegin(); iter != layouts.cend(); iter++)
				if (ImGui::Selectable(iter->c_str(), *iter == layoutName_)) {
					layoutName_ = *iter;
					KsThemeManager::singleton().applyLayout(layoutName_, &tp);
				}

			ImGui::EndCombo();
		}
	}

	auto canvas = KsThemeManager::singleton().listCanvas();
	if (!canvas.empty()) {

		auto iter = std::find(canvas.cbegin(), canvas.cend(), canvasName_);
		//if (iter == canvas.cend()) {
		//	canvasName_ = canvas.front();
		//	KsThemeManager::singleton().applyTheme(canvasName_, &tp);
		//}

		if (ImGui::BeginCombo("Canvas", canvasName_.c_str(), ImGuiComboFlags_HeightLarge)) {
			for (iter = canvas.cbegin(); iter != canvas.cend(); iter++)
				if (ImGui::Selectable(iter->c_str(), *iter == canvasName_)) {
					canvasName_ = *iter;
					KsThemeManager::singleton().applyCanvas(canvasName_, &tp);
				}

			ImGui::EndCombo();
		}
	}

	auto palettes = KsThemeManager::singleton().listPalettes();
	if (!palettes.empty()) {

		auto iter = std::find(palettes.cbegin(), palettes.cend(), paletteName_);
		//if (iter == palettes.cend()) {
		//	paletteName_ = palettes.front();
		//	KsThemeManager::singleton().applyTheme(paletteName_, &tp);
		//}

		if (ImGui::BeginCombo("Palette", paletteName_.c_str(), ImGuiComboFlags_HeightLarge)) {
			if (ImGui::TreeNodeEx("Group")) {
				for (iter = palettes.cbegin(); iter != palettes.cend(); iter++)
					if (ImGui::Selectable(iter->c_str(), *iter == paletteName_)) {
						paletteName_ = *iter;
						KsThemeManager::singleton().applyPalette(paletteName_, &tp);
					}

				ImGui::TreePop();
			}

			ImGui::EndCombo();
		}
	}

	ImGui::Separator();
}


void KvRdPlot::applyTheme_(const std::string& name, KvThemedPlot* plot)
{
	themeName_ = name;

	auto& themeMgr = KsThemeManager::singleton();
	canvasName_ = themeMgr.canvasName(name);
	layoutName_ = themeMgr.layoutName(name);
	paletteName_ = themeMgr.paletteName(name);

	themeMgr.applyTheme(name, plot);
}