#include "KcRdPlot1d.h"
#include <assert.h>
#include "KvData.h"
#include "KtSampling.h"
#include "KvContinued.h"


KcRdPlot1d::KcRdPlot1d()
	: type_(k_line)
	, KvDataRender("plot1d")
{

}


void KcRdPlot1d::pushData(std::shared_ptr<KvData> data)
{

}