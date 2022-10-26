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


bool KvDataOperator::isStream(kIndex outPort) const
{
	assert(!inputs_.empty() && inputs_.size() == inPorts());

	bool streaming(false);
	for (auto i : inputs_)
		if (i) {
			auto prov = std::dynamic_pointer_cast<KvDataProvider>(i->parent().lock());
			if (prov && prov->isStream(i->index()))
			    return true; // ��һ��������stream���򷵻�true
		}

	return false;
}


kIndex KvDataOperator::dim(kIndex outPort) const
{
	assert(!inputs_.empty() && inputs_.size() == inPorts());

	auto d = kPrivate::first_non_null(inputs_);
	if (d == nullptr)
		return 0;

	auto prov = std::dynamic_pointer_cast<KvDataProvider>(d->parent().lock());
	return prov ? prov->dim(d->index()) : 0;
}


kIndex KvDataOperator::channels(kIndex outPort) const
{
	assert(!inputs_.empty() && inputs_.size() == inPorts());

	auto d = kPrivate::first_non_null(inputs_);
	if (d == nullptr)
		return 0;

	auto prov = std::dynamic_pointer_cast<KvDataProvider>(d->parent().lock());
	return prov ? prov->channels(d->index()) : 0;
}


kRange KvDataOperator::range(kIndex outPort, kIndex axis) const
{
	assert(!inputs_.empty() && inputs_.size() == inPorts());

	auto d = kPrivate::first_non_null(inputs_);
	if (d == nullptr)
		return kRange{ 0, 1 };

	auto prov = std::dynamic_pointer_cast<KvDataProvider>(d->parent().lock());

	return prov ? prov->range(d->index(), axis) : kRange{ 0, 1 };
}


kReal KvDataOperator::step(kIndex outPort, kIndex axis) const
{
	assert(!inputs_.empty() && inputs_.size() == inPorts());

	auto d = kPrivate::first_non_null(inputs_);
	if (d == nullptr)
		return 0;

	auto prov = std::dynamic_pointer_cast<KvDataProvider>(d->parent().lock());

	return prov ? prov->step(d->index(), axis) : 0;
}


kIndex KvDataOperator::size(kIndex outPort, kIndex axis) const
{
	assert(!inputs_.empty() && inputs_.size() == inPorts());

	auto d = kPrivate::first_non_null(inputs_);
	if (d == nullptr)
		return 0;

	auto prov = std::dynamic_pointer_cast<KvDataProvider>(d->parent().lock());

	return prov ? prov->size(d->index(), axis) : 0;
}


bool KvDataOperator::onNewLink(KcPortNode* from, KcPortNode* to)
{
	if (to->parent().lock().get() == this) { // ֻ������������
		assert(to->index() < inputs_.size());
		
		auto prov = std::dynamic_pointer_cast<KvDataProvider>(from->parent().lock());
		if (prov == nullptr  // ֻ����provider�ڵ����
			|| inputs_[to->index()]) // ÿ������˿�ֻ����������
			return false; 

		inputs_[to->index()] = from;
	}

	return true;
}


void KvDataOperator::onDelLink(KcPortNode* from, KcPortNode* to)
{
	if (to->parent().lock().get() == this) { // ֻ������������
		assert(to->index() < inputs_.size());
		assert(from == inputs_[to->index()]);

		inputs_[to->index()] = nullptr;
	}
}


void KvDataOperator::onInput(KcPortNode* outPort, unsigned inPort)
{
	assert(outPort && inPort < inputs_.size());
	assert(inputs_[inPort] == outPort);

	auto prov = std::dynamic_pointer_cast<KvDataProvider>(outPort->parent().lock());
	assert(prov);

	idata_[inPort] = prov->fetchData(outPort->index());
}


std::shared_ptr<KvData> KvDataOperator::fetchData(kIndex outPort) const
{
	assert(outPort < odata_.size());

	return odata_[outPort];
}
