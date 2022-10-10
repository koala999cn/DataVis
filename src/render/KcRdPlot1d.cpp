#include "KcRdPlot1d.h"
#include "imapp/KcImPlot1d.h"
#include "imapp/KsImApp.h"
#include "imapp/KgImWindowManager.h"
#include "imapp/KcImPlottable.h"
#include "prov/KvDataProvider.h"


KcRdPlot1d::KcRdPlot1d()
	: type_(k_line)
	, KvDataRender("plot1d")
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

	auto pos = portId2Idx_.find(outPort->id());
	assert(pos != portId2Idx_.end());

	auto data = prov->fetchData(outPort->index());
	plot1d_->plottable(pos->second)->data() = data;
}


bool KcRdPlot1d::onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins)
{
	if (ins.empty())
		return true;

	// 根据输入构造plottables
	portId2Idx_.clear();
	plot1d_->removeAllPlottables();
	for (auto& i : ins) {
		assert(i.first == 0); // 只有1个输入端口
		auto name = i.second->parent().lock()->name();
		plot1d_->addPlottable(new KcImPlottable(name));
		portId2Idx_.insert({i.second->id(), plot1d_->plottableCount() - 1});
	}

	plot1d_->setVisible(true);
	return true;
}

