#include "KvRdPlot.h"
#include "plot/KvPlot.h"
#include "plot/KvPlottable.h"
#include "prov/KvDataProvider.h"
#include "dsp/KcMonoDiscreted.h"
#include "dsp/KcMonoContinued.h"
#include "imapp/KvImWindow.h"
#include "imapp/KsImApp.h"
#include "imapp/KgImWindowManager.h"
#include "imguix.h"
#include "KuStrUtil.h"
#include "misc/cpp/imgui_stdlib.h"
#include "plot/KsThemeManager.h"
#include "plot/KcThemedPlotImpl_.h"
#include "plot/KvCoord.h"
#include "plot/KcLegend.h"
#include "plot/KcColorBar.h"
#include "KvNode.h"
#include "KcSampled1d.h"
#include "KcSampled2d.h"


namespace kPrivate
{
	bool TreePush(const char* label)
	{
		return ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding);
	}

	void TreePop()
	{
		ImGui::TreePop();
	}
}


KvRdPlot::KvRdPlot(const std::string_view& name, const std::shared_ptr<KvPlot>& plot)
	: KvDataRender(name)
	, plot_(plot)
{
	plot_->setVisible(false);

	auto imWindow = std::dynamic_pointer_cast<KvImWindow>(plot_);
	if (imWindow)
		KsImApp::singleton().windowManager().registerWindow(imWindow);
}


KvRdPlot::~KvRdPlot()
{
	auto imWindow = std::dynamic_pointer_cast<KvImWindow>(plot_);
	if (imWindow)
		KsImApp::singleton().windowManager().releaseWindow(imWindow);

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
	
	if (prov->isStream(outPort->index())) { // shifting the data

		assert(prov->isSampled(outPort->index()) || 
			prov->isArray(outPort->index())); // TODO: other types

		auto xrange = plot_->coord().upper().x() - plot_->coord().lower().x();
		streaming_(streamData_[outPort], data, xrange);
		data = streamData_[outPort];
	}

	if (data->channels() == 1 || numPlts == 1) {
		r.first->second->setData(data);
	}
	else {
		assert(data->channels() == numPlts);

		kIndex ch(0);

		if (data->isDiscreted()) {
			auto disc = std::dynamic_pointer_cast<KvDiscreted>(data);
			for (auto i = r.first; i != r.second; i++) {
				assert(i->first == outPort->id());
				i->second->setData(std::make_shared<KcMonoDiscreted>(disc, ch++));
			}
		}
		else {
			auto cont = std::dynamic_pointer_cast<KvContinued>(data);
			for (auto i = r.first; i != r.second; i++) {
				assert(i->first == outPort->id());
				i->second->setData(std::make_shared<KcMonoContinued>(cont, ch++));
			}
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

	return !working_(); // 运行时不接受新的链接
}


void KvRdPlot::onDelLink(KcPortNode* from, KcPortNode* to)
{
	assert(to->parent().lock().get() == this && to->index() == 0);

	auto pnode = from->parent().lock();
	assert(pnode);

	auto prov = std::dynamic_pointer_cast<KvDataProvider>(pnode);
	assert(prov);

	auto r = port2Plts_.equal_range(from->id());
	if (r.first != r.second) {
		for (auto i = r.first; i != r.second; i++)
			plot_->removePlottable(i->second);
		port2Plts_.erase(from->id());
	}

	if (prov->isStream(from->index()))
		streamData_.erase(from);
}


bool KvRdPlot::onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins)
{
	plot_->removeAllPlottables();
	plot_->autoFit() = true;
	port2Plts_.clear(); 
	streamData_.clear();

	if (!ins.empty()) {
		// 根据输入配置plot

		typename KvCoord::point3 
			lower(std::numeric_limits<typename KvCoord::float_t>::max()), 
			upper(std::numeric_limits<typename KvCoord::float_t>::lowest());

		for (unsigned i = 0; i < ins.size(); i++) {
			auto port = ins[i].second;
			auto node = port->parent().lock();
			if (!node) continue;

			auto prov = dynamic_cast<KvDataProvider*>(node.get());
			assert(prov);
			for (unsigned j = 0; j < std::min<unsigned>(prov->dim(port->index()) + 1, 3); j++) {
				auto r = prov->range(port->index(), j);
				if (lower[j] > r.low())
					lower[j] = r.low();
				if (upper[j] < r.high())
					upper[j] = r.high();
			}

			if (lower.z() == std::numeric_limits<typename KvCoord::float_t>::max()) // 输入全是二维数据
				lower.z() = upper.z() = 0; 

			auto plts = createPlottable_(port);
			if (plts.empty())
				return false;

			for (auto plt : plts) {
				plot_->addPlottable(plt);
				port2Plts_.insert(std::make_pair(port->id(), plt));
			}

			if (prov->isStream(port->index())) {
				if (prov->dim(port->index()) == 1)
					streamData_[port] = std::make_shared<KcSampled1d>(
						prov->step(port->index(), 0), 
						prov->channels(port->index()));
				else if (prov->dim(port->index()) == 2) {
					auto data = std::make_shared<KcSampled2d>(
						prov->step(port->index(), 0), 
						prov->step(port->index(), 1), 
						prov->channels(port->index()));

					data->resize(0, prov->size(port->index(), 1));
					streamData_[port] = data;
				}
			}

			// 如果有1个dynamic输入，则将autofit置false
			plot_->autoFit() &= !prov->isDynamic(port->index());
		}
		
		// update theme
		updateTheme_();

		plot_->coord().setExtents(lower, upper);
		plot_->setVisible(true);
	}

	return true;
}


void KvRdPlot::showProperySet()
{
	super_::showProperySet(); // show the name property
	ImGui::Separator();

	showPlotProperty_();
	ImGui::Separator();

	showThemeProperty_();
	ImGui::Separator();

	showCoordProperty_();
	ImGui::Separator();

	if (plot_->legend()->itemCount() > 0) {
		showLegendProperty_();
		ImGui::Separator();
	}

	if (plot_->colorBar()) {
		showColorBarProperty_();
		ImGui::Separator();
	}

	showPlottableProperty_();
}


void KvRdPlot::showPlotProperty_()
{
	if (kPrivate::TreePush("Plot")) {
		bool vis = plot_->visible();
		if (ImGuiX::prefixCheckbox("##Plot", &vis))
			plot_->setVisible(vis);
		ImGui::InputText("Title", &plot_->title());
		ImGui::PopItemWidth(); // match for prefixCheckbox

		ImGui::ColorEdit4("Background", plot_->background().color);

		ImGui::Checkbox("Auto Fit", &plot_->autoFit());

		kPrivate::TreePop();
	}
}


void KvRdPlot::showThemeProperty_()
{
	auto& themeMgr = KsThemeManager::singleton();
	if (themeMgr.empty())
		return;

	if (!kPrivate::TreePush("Design"))
		return;

	auto groups = themeMgr.listGroups();
	groups.insert(groups.cbegin(), ""); // insert the global group
	static const char* labels[] = { "Theme", "Layout", "Canvas", "Palette" };

	for (int i = 0; i < 4; i++) {
		if (themeMgr.isEmpty(i))
			continue;

		if (!ImGui::BeginCombo(labels[i], curTheme_[i].second.c_str(), ImGuiComboFlags_HeightLarge))
			continue;

		for (unsigned g = 0; g < groups.size(); g++) {

			auto names = themeMgr.listNames(i, groups[g]);
			if (names.empty())
				continue;

			if (!groups[g].empty() && !ImGui::TreeNodeEx(groups[g].c_str()))
			    continue;
		
			for (auto& name : names) {
				if (ImGui::Selectable(name.c_str(), name == curTheme_[i].second && groups[g] == curTheme_[i].first)) {
					curTheme_[i].second = name;
					curTheme_[i].first = groups[g];
					onThemeChanged(i);
				}

				if (name == curTheme_[i].second && groups[g] == curTheme_[i].first)
					ImGui::SetItemDefaultFocus();

				// 绘制色带
				if (i == KsThemeManager::k_canvas) {
					ImGui::SameLine();
					auto canvas = KsThemeManager::singleton().getCanvas(groups[g], name);
					ImGuiX::drawColorBar(canvas, ImGui::GetCursorScreenPos(),
						{ ImGui::GetTextLineHeight(), ImGui::GetTextLineHeight() }, 1);
				}
				else if (i == KsThemeManager::k_palette) {
					ImGui::SameLine();
					std::vector<color4f> majors, minors;
					KsThemeManager::singleton().getPalette(groups[g], name, majors, minors);
					ImGuiX::drawColorBar(majors, ImGui::GetCursorScreenPos(),
						{ 6, ImGui::GetTextLineHeight() }, 2);
				}
			}

			if (!groups[g].empty())
				ImGui::TreePop();
		}

		ImGui::EndCombo();
	}

	kPrivate::TreePop();
}


void KvRdPlot::showCoordProperty_()
{
	if (!kPrivate::TreePush("Axes"))
		return;

	auto lower = point3f(plot_->coord().lower());
	auto upper = point3f(plot_->coord().upper());
	auto speed = (upper - lower) * 0.1;
	for (unsigned i = 0; i < speed.size(); i++)
		if (speed.at(i) == 0)
			speed.at(i) = 1;

	bool extendsChanged(false);
	if (ImGui::DragFloatRange2("X", &lower.x(), &upper.x(), speed.x()))
		extendsChanged = true;
	if (ImGui::DragFloatRange2("Y", &lower.y(), &upper.y(), speed.y()))
		extendsChanged = true;
	if (ImGui::DragFloatRange2("Z", &lower.z(), &upper.z(), speed.z()))
		extendsChanged = true;

	if (extendsChanged) {
		plot_->coord().setExtents(lower, upper);
		plot_->autoFit() = false;
	}

	kPrivate::TreePop();
}


void KvRdPlot::showLegendProperty_()
{
	if (!kPrivate::TreePush("Legend"))
		return;

	ImGui::Checkbox("Show", &plot_->showLegend());

	auto& loc = plot_->legend()->location();
	ImGuiX::alignment("Alignment", loc);

	kPrivate::TreePop();
}



void KvRdPlot::showColorBarProperty_()
{
	if (!kPrivate::TreePush("ColorBar"))
		return;

	ImGui::Checkbox("Show", &plot_->showColorBar());

	auto& loc = plot_->colorBar()->location();
	ImGuiX::alignment("Alignment", loc);

	kPrivate::TreePop();
}


void KvRdPlot::showPlottableProperty_()
{
	if (plot_->plottableCount() > 0) {

		if (!kPrivate::TreePush("Plottable(s)"))
			return;

		for (unsigned idx = 0; idx < plot_->plottableCount(); idx++) {

			auto plt = plot_->plottableAt(idx);

			std::string label = "##Plottable" + KuStrUtil::toString(idx);
			ImGui::Checkbox(label.c_str(), &plt->visible());

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
			ImGui::SameLine();
			label = "##Node" + KuStrUtil::toString(idx);
			bool show = ImGui::TreeNode(label.c_str());
			ImGui::PopStyleVar();

			ImGui::SameLine();
			ImGui::PushID(plt);
			ImGui::InputText("##", &plt->name());
			ImGui::PopID();

			std::vector<color4f> majors(plt->majorColors());
			for (unsigned i = 0; i < plt->majorColors(); i++)
				majors[i] = plt->majorColor(i);

			for (unsigned i = 0; i < plt->majorColors(); i++) {
				ImGui::SameLine();
				ImGui::PushID(plt + 1 + i);
				if (ImGui::ColorEdit4("##", majors[i],
					ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
					plt->setMajorColors(majors);
				ImGui::PopID();
			}

			if (show) {
				showPlottableTypeProperty_(idx);
				showPlottableSampCountProperty_(idx);
				showPlottableSpecificProperty_(idx);
				ImGui::TreePop();
			}
		}

		kPrivate::TreePop();
	}
}


void KvRdPlot::showPlottableTypeProperty_(unsigned idx)
{
	int type = plottableType_(plot_->plottableAt(idx));

	if (ImGui::BeginCombo("Type", plottableTypeStr_(type))) {
		for (int i = 0; i < supportPlottableTypes_(); i++)
			if (ImGui::Selectable(plottableTypeStr_(i), i == type)) {
				auto oldPlt = plot_->plottableAt(idx);
				auto newPlt = newPlottable_(i, oldPlt->name());

				// clone the theme
				std::vector<color4f> majorColors(oldPlt->majorColors());
				for (unsigned c = 0; c < majorColors.size(); c++)
					majorColors[c] = oldPlt->majorColor(c);
				newPlt->setMajorColors(majorColors);
				if (newPlt->minorColorNeeded() && oldPlt->minorColorNeeded())
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


void KvRdPlot::showPlottableSampCountProperty_(unsigned idx)
{
	auto data = plot_->plottableAt(idx)->data();
	if (data && data->isContinued()) {
		unsigned minCount(1), maxCount(std::pow(1024 * 1024, 1. / data->dim()));
		ImGui::DragScalarN("Sampling Count", ImGuiDataType_U32,
			&plot_->plottableAt(idx)->sampCount(0), data->dim(), 1,
			&minCount, &maxCount);
	}
}


void KvRdPlot::onThemeChanged(int type)
{
	auto& themeMgr = KsThemeManager::singleton();
	KcThemedPlotImpl_ tp(*plot_);

	switch (type)
	{
	case KsThemeManager::k_theme:
		themeMgr.applyTheme(curTheme_[type].first, curTheme_[type].second, &tp);

		// 同步包含的canvas, layout, palette等主题选项
		curTheme_[KsThemeManager::k_canvas].second = themeMgr.canvasName(curTheme_[type].first, curTheme_[type].second);
		curTheme_[KsThemeManager::k_layout].second = themeMgr.layoutName(curTheme_[type].first, curTheme_[type].second);
		curTheme_[KsThemeManager::k_palette].second = themeMgr.paletteName(curTheme_[type].first, curTheme_[type].second);

		// TODO: adjust the group name
		curTheme_[KsThemeManager::k_canvas].first = curTheme_[KsThemeManager::k_layout].first =
			curTheme_[KsThemeManager::k_palette].first = curTheme_[type].first;
		break;

	case KsThemeManager::k_layout:
		themeMgr.applyLayout(curTheme_[type].first, curTheme_[type].second, &tp);
		break;

	case KsThemeManager::k_canvas:
		themeMgr.applyCanvas(curTheme_[type].first, curTheme_[type].second, &tp);
		break;

	case KsThemeManager::k_palette:
		themeMgr.applyPalette(curTheme_[type].first, curTheme_[type].second, &tp);
		break;

	default:
		assert(false);
		break;
	}
}


void KvRdPlot::onDoubleClicked()
{
	if (plot_)
		plot_->setVisible(true);
}


void KvRdPlot::updateTheme_()
{
	if (!curTheme_[KsThemeManager::k_theme].second.empty()) {
		KcThemedPlotImpl_ tp(*plot_);

		// TODO: 这种方式不能完全复现
		KsThemeManager::singleton().applyTheme(
			curTheme_[KsThemeManager::k_theme].first, curTheme_[KsThemeManager::k_theme].second, &tp);
	}
}


namespace kPrivate
{
	template<int DIM>
	static void streaming_(std::shared_ptr<KvData> curData, std::shared_ptr<KvData> newData, double xrange)
	{
		assert(curData->dim() == newData->dim() && curData->channels() == newData->channels());

		auto samp = std::dynamic_pointer_cast<KtSampledArray<DIM>>(curData);
		assert(samp);
		if (xrange == 0) {
			samp->clear();
			return;
		}

		auto cnt = xrange / samp->step(0);
		auto sampArray = std::dynamic_pointer_cast<KtSampledArray<DIM>>(newData);
		if (sampArray) { // 使用快速版本
			samp->shift(*sampArray, cnt);
		}
		else { // 使用通用版本
			samp->shift(*std::dynamic_pointer_cast<KvSampled>(newData), cnt);
		}
	}
}

void KvRdPlot::streaming_(std::shared_ptr<KvData> curData, std::shared_ptr<KvData> newData, double xrange)
{
	if (curData->dim() == 1) 
		kPrivate::streaming_<1>(curData, newData, xrange);
	else if (curData->dim() == 2) 
		kPrivate::streaming_<2>(curData, newData, xrange);
	else {
		assert(false);
	}
}