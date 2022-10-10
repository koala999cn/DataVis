#include "KcRdPlot1d.h"
#include "imapp/KcImPlot1d.h"
#include "imapp/KsImApp.h"
#include "imapp/KgImWindowManager.h"
#include "imapp/KcImPlottable.h"
#include "prov/KvDataProvider.h"
#include "dsp/KcDataMono.h"
#include "KuStrUtil.h"


KcRdPlot1d::KcRdPlot1d()
	: KvDataRender("plot1d")
{
	plot1d_ = std::make_shared<KcImPlot1d>("Plot1d");
	plot1d_->setVisible(false);
	KsImApp::singleton().windowManager().registerDynamic(plot1d_);
}


KcRdPlot1d::~KcRdPlot1d()
{
	KsImApp::singleton().windowManager().releaseDynamic(plot1d_);
	plot1d_ = nullptr;
}


void KcRdPlot1d::onInput(KcPortNode* outPort, unsigned inPort)
{
	assert(inPort == 0);
    
	auto pnode = outPort->parent().lock();
	assert(pnode);

	auto prov = std::dynamic_pointer_cast<KvDataProvider>(pnode);
	assert(prov);

	auto r = port2Plts_.equal_range(outPort->id());
	assert(r.first != r.second);

	auto data = prov->fetchData(outPort->index());
	assert(data->channels() == std::distance(r.first, r.second));

	if (data->channels() == 1)
		r.first->second->data() = data;
	else {
		kIndex ch(0);
		auto disc = std::dynamic_pointer_cast<KvDiscreted>(data);
		assert(disc);

		for (auto i = r.first; i != r.second; i++) {
			assert(i->first == outPort->id());
			i->second->data() = std::make_shared<KcDataMono>(disc, ch++);
		}
	}
}


bool KcRdPlot1d::onNewLink(KcPortNode* from, KcPortNode* to)
{
	assert(to->parent().lock().get() == this && to->index() == 0);

	auto pnode = from->parent().lock();
	assert(pnode);

	auto prov = std::dynamic_pointer_cast<KvDataProvider>(pnode);
	assert(prov);

	if (prov->channels() == 1) {
		auto plt = new KcImPlottable(pnode->name());
		plot1d_->addPlottable(plt);
		port2Plts_.insert(std::make_pair(from->id(), plt));
	}
	else {
		for (kIndex ch = 0; ch < prov->channels(); ch++) {
			std::string name = pnode->name() + " - ch" + KuStrUtil::toString(ch);
			auto plt = new KcImPlottable(name);
			plot1d_->addPlottable(plt);
			port2Plts_.insert(std::make_pair(from->id(), plt));
		}
	}

	return true;
}


void KcRdPlot1d::onDelLink(KcPortNode* from, KcPortNode* to)
{
	assert(to->parent().lock().get() == this && to->index() == 0);

	auto pnode = from->parent().lock();
	assert(pnode);

	auto prov = std::dynamic_pointer_cast<KvDataProvider>(pnode);
	assert(prov);

	auto r = port2Plts_.equal_range(from->id());
	assert(r.first != r.second);

	for (auto i = r.first; i != r.second; i++)
		plot1d_->removePlottable(i->second);

	port2Plts_.erase(from->id());
}


bool KcRdPlot1d::onStartPipeline()
{
	plot1d_->setVisible(true);
	return true;
}

