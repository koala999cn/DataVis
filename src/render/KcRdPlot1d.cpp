#include "KcRdPlot1d.h"
#include "imapp/KcImPlot1d.h"
#include "imapp/KsImApp.h"
#include "imapp/KgImWindowManager.h"


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
}


void KcRdPlot1d::pushData(std::shared_ptr<KvData> data)
{
	plot1d_->setVisible(true);
}
