#include "KcPvContinued.h"
#include "KvData.h"
#include <QPointF>
#include "KtSampler.h"
#include <assert.h>


KcPvContinued::KcPvContinued(const QString& name, std::shared_ptr<KvData> data)
	: KcPvSampled(name, data->dim() == 1 ? 
	    std::shared_ptr<KvData>(new KtSampler<1>(data)) : data->dim() == 2 ?
		std::shared_ptr<KvData>(new KtSampler<2>(data)) :
		std::shared_ptr<KvData>(new KtSampler<3>(data))
	)
	, data_(data) 
{
	assert(data->isContinued());

	// 初始化采样参数, 各维度1000个采样点，采样间隔0.001
	auto samp = std::dynamic_pointer_cast<KvSampled>(this->data());
	kIndex shape[3] = { 1000, 1000, 1000 };
	samp->resize(shape);
	for (kIndex i = 0; i < data->dim(); i++)
		samp->reset(i, 0, 0.001);
}
