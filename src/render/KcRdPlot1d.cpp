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
	plot1d_->addPlottable(new KcImPlottable("test"));
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

	auto data = prov->fetchData(outPort->index());
	plot1d_->plottable(0)->data() = data;
}


bool KcRdPlot1d::onStartPipeline()
{
	plot1d_->setVisible(true);
	return true;
}

