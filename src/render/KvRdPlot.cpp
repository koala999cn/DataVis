#include "KvRdPlot.h"
#include "plot/KvPlot.h"
#include "plot/KvPlottable.h"
#include "prov/KvDataProvider.h"
#include "dsp/KcDataMono.h"
#include "imapp/KvImWindow.h"
#include "imapp/KsImApp.h"
#include "imapp/KgImWindowManager.h"
#include "imgui.h"


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
		r.first->second->data() = data;
	}
	else {
		assert(data->channels() == numPlts);

		kIndex ch(0);
		auto disc = std::dynamic_pointer_cast<KvDiscreted>(data);
		assert(disc);

		for (auto i = r.first; i != r.second; i++) {
			assert(i->first == outPort->id());
			i->second->data() = std::make_shared<KcDataMono>(disc, ch++);
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
	if (ImGui::Checkbox("##", &vis))
		plot_->setVisible(vis);

	ImGui::SameLine();
	super_::showProperySet();

	ImGui::ColorEdit4("Background", (float*)&plot_->background().r());

	ImGui::Checkbox("Auto Fit", &plot_->autoFit());

	if (plot_->plottableCount() > 0) {

		if (ImGui::TreeNode("Plottable(s)")) {
			for (unsigned ch = 0; ch < plot_->plottableCount(); ch++) {
				auto plt = plot_->plottable(ch);

				if (ImGui::TreeNode(plt->name().c_str())) {
					ImGui::ColorEdit4("Major Color", &plt->majorColor(0).r());
					ImGui::TreePop();
				}
			}

			ImGui::TreePop();
		}

	}
}