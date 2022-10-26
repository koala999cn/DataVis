#include "KcPvData.h"
#include "KvDiscreted.h"


KcPvData::KcPvData(const std::string_view& name, std::shared_ptr<KvData> data)
	: KvDataProvider(name), data_(data) 
{

}


kIndex KcPvData::dim(kIndex outPort) const
{
	return data_->dim();
}


kIndex KcPvData::channels(kIndex outPort) const
{
	return data_->channels();
}


kRange KcPvData::range(kIndex outPort, kIndex axis) const
{
	return data_->range(axis);
}


kReal KcPvData::step(kIndex outPort, kIndex axis) const
{
	auto dis = std::dynamic_pointer_cast<KvDiscreted>(data_);
	return dis ? dis->step(axis) : 0;
}


kIndex KcPvData::size(kIndex outPort, kIndex axis) const
{
	auto dis = std::dynamic_pointer_cast<KvDiscreted>(data_);
	return dis ? dis->size(axis) : KvData::k_inf_size;
}


std::shared_ptr<KvData> KcPvData::fetchData(kIndex outPort) const
{
	assert(outPort == 0);
	return data_;
}


namespace kPrivate
{
	enum KeDataProperty
	{
		k_range
	};
}
