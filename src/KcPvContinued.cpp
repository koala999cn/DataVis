#include "KcPvContinued.h"
#include "KvData.h"
#include <QPointF>
#include "KtSampler.h"
#include <assert.h>


KcPvContinued::KcPvContinued(const QString& name, std::shared_ptr<KvData> data)
	: KcPvSampled(name, data->dim() == 1 ? 
	    std::make_shared<KtSampler<1>>(data) : (data->dim() == 2 ? 
		std::make_shared<KtSampler<2>>(data) :
		std::make_shared<KtSampler<3>>(data))
	)
	, data_(data) 
{
	assert(data->isContinued());
}
