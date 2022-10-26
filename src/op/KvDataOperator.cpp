#include "KvDataOperator.h"
#include "KvDiscreted.h"


namespace kPrivate
{
	template<typename VEC_T>
	typename VEC_T::value_type first_non_null(const VEC_T& v)
	{
		for (auto& i : v)
			if (i) return i;

		return nullptr;
	}
}


bool KvDataOperator::isStream() const
{
	return false;
}


kIndex KvDataOperator::dim() const
{
	assert(!outputs_.empty() && outputs_.size() == outPorts());

	auto d = kPrivate::first_non_null(outputs_);
	return d ? d->dim() : 0;
}


kIndex KvDataOperator::channels() const
{
	assert(!outputs_.empty() && outputs_.size() == outPorts());

	auto d = kPrivate::first_non_null(outputs_);
	return d ? d->channels() : 0;
}


kRange KvDataOperator::range(kIndex axis) const
{
	assert(!outputs_.empty() && outputs_.size() == outPorts());

	auto d = kPrivate::first_non_null(outputs_);
	return d ? d->range(axis) : kRange{ 0, 0 };
}


kReal KvDataOperator::step(kIndex axis) const
{
	assert(!outputs_.empty() && outputs_.size() == outPorts());

	auto d = std::dynamic_pointer_cast<KvDiscreted>(kPrivate::first_non_null(outputs_));
	return d ? d->step(axis) : 0;
}


kIndex KvDataOperator::size(kIndex axis) const
{
	assert(!outputs_.empty() && outputs_.size() == outPorts());

	auto d = kPrivate::first_non_null(outputs_);
	return d ? d->size() : 0;
}


bool KvDataOperator::onNewLink(KcPortNode* from, KcPortNode* to)
{
	if (to->parent().lock().get() == this) { // 只处理输入连接
		assert(to->index() < inputs_.size());
		
		auto prov = std::dynamic_pointer_cast<KvDataProvider>(from->parent().lock());
		if (prov == nullptr) 
			return false; // 只允许provider节点接入

		onInput(from, to->index());
	}

	return true;
}


void KvDataOperator::onDelLink(KcPortNode* from, KcPortNode* to)
{
	if (to->parent().lock().get() == this) { // 只处理输入连接
		assert(to->index() < inputs_.size());

		auto prov = std::dynamic_pointer_cast<KvDataProvider>(from->parent().lock());
		assert(prov);

		inputs_[to->index()] = nullptr;
	}
}


void KvDataOperator::onInput(KcPortNode* outPort, unsigned inPort)
{
	assert(outPort && inPort < inputs_.size());

	auto prov = std::dynamic_pointer_cast<KvDataProvider>(outPort->parent().lock());
	assert(prov);

	inputs_[inPort] = prov->fetchData(outPort->index());
}


std::shared_ptr<KvData> KvDataOperator::fetchData(kIndex outPort) const
{
	assert(outPort < outputs_.size());

	return outputs_[outPort];
}
