#include "KvDataOperator.h"
#include "KvDiscreted.h"


int KvDataOperator::spec(kIndex) const
{
	assert(!inputs_.empty() && inputs_.size() == inPorts());
	return inputSpec_();
}


kRange KvDataOperator::range(kIndex outPort, kIndex axis) const
{
	assert(outPort < odata_.size());
	assert(!inputs_.empty() && inputs_.size() == inPorts());

	if (axis < dim(outPort)) {
		if (odata_[outPort])
			return odata_[outPort]->range(axis);

		return inputRange_(axis);
	}
	else { // value range：优先从prov获取
		return inputRange_(axis);
	}
}


kReal KvDataOperator::step(kIndex outPort, kIndex axis) const
{
	assert(outPort < odata_.size());
	assert(!inputs_.empty() && inputs_.size() == inPorts());

	if (odata_[outPort]) {
		auto disc = std::dynamic_pointer_cast<KvDiscreted>(odata_[outPort]);
		return disc ? disc->step(axis) : 0;
	}

	return inputStep_(axis);
}


kIndex KvDataOperator::size(kIndex outPort, kIndex axis) const
{
	assert(outPort < odata_.size());
	assert(!inputs_.empty() && inputs_.size() == inPorts());

	if (odata_[outPort]) {
		auto disc = std::dynamic_pointer_cast<KvDiscreted>(odata_[outPort]);
		return disc ? disc->size(axis) : 0;
	}

	return inputSize_(axis);
}


bool KvDataOperator::onNewLink(KcPortNode* from, KcPortNode* to)
{
	if (to->parent().lock().get() == this) { // 只处理输入连接
		assert(to->index() < inputs_.size());
		
		auto prov = std::dynamic_pointer_cast<KvDataProvider>(from->parent().lock());
		if (prov == nullptr  // 只允许provider节点接入
			|| inputs_[to->index()]) // 每个输入端口只允许单个接入
			return false; 

		if (!permitInput(prov->spec(from->index()), to->index()))
			return false; // 不接受的数据规格

		inputs_[to->index()] = from;
	}

	return true;
}


void KvDataOperator::onDelLink(KcPortNode* from, KcPortNode* to)
{
	if (to->parent().lock().get() == this) { // 只处理输入连接
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

	if (prov->dataStamp(outPort->index()) > idataStamps_[inPort]){ // 是否有更新的数据
		idata_[inPort] = prov->fetchData(outPort->index());
		idataStamps_[inPort] = prov->dataStamp(outPort->index());
	}
}


std::shared_ptr<KvData> KvDataOperator::fetchData(kIndex outPort) const
{
	assert(outPort < odata_.size());

	return odata_[outPort];
}


unsigned KvDataOperator::dataStamp(kIndex outPort) const
{
	return odataStamps_[outPort];
}


bool KvDataOperator::onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins)
{
	if (ins.size() != inPorts())
		return false; // 输入未全部连接，不启动

	for (auto& i : ins) {
		auto inPort = i.first;
		auto inNode = i.second;
		assert(inPort < inputs_.size() && inNode == inputs_[inPort]);

		auto prov = std::dynamic_pointer_cast<KvDataProvider>(inNode->parent().lock());
		if (prov == nullptr)
			return false; // we'll never touch here

		assert(permitInput(prov->spec(inNode->index()), inPort));
	}

	idataStamps_.assign(inPorts(), 0);
	odataStamps_.assign(outPorts(), 0);

	return true;
}


int KvDataOperator::inputSpec_(kIndex inPort) const
{
	assert(inPort < inputs_.size() && inPort >= 0);

	auto d = inputs_[inPort];
	if (d == nullptr)
		return 0;

	auto prov = std::dynamic_pointer_cast<KvDataProvider>(d->parent().lock());
	assert(prov);
	return prov->spec(d->index());
}


kRange KvDataOperator::inputRange_(kIndex inPort, kIndex axis) const
{
	assert(inPort < inputs_.size() && inPort >= 0);

	auto d = inputs_[inPort];
	if (d == nullptr)
		return kRange(0, 0);

	auto prov = std::dynamic_pointer_cast<KvDataProvider>(d->parent().lock());
	assert(prov);
	return prov->range(d->index(), axis);
}


kReal KvDataOperator::inputStep_(kIndex inPort, kIndex axis) const
{
	assert(inPort < inputs_.size() && inPort >= 0);

	auto d = inputs_[inPort];
	if (d == nullptr)
		return 0;

	auto prov = std::dynamic_pointer_cast<KvDataProvider>(d->parent().lock());
	assert(prov);
	return prov->step(d->index(), axis);
}


kIndex KvDataOperator::inputSize_(kIndex inPort, kIndex axis) const
{
	assert(inPort < inputs_.size() && inPort >= 0);

	auto d = inputs_[inPort];
	if (d == nullptr)
		return 0;

	auto prov = std::dynamic_pointer_cast<KvDataProvider>(d->parent().lock());
	assert(prov);
	return prov->size(d->index(), axis);
}


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


int KvDataOperator::inputSpec_() const
{
	auto d = kPrivate::first_non_null(inputs_);
	if (d == nullptr)
		return 0;

	auto prov = std::dynamic_pointer_cast<KvDataProvider>(d->parent().lock());
	assert(prov);
	return prov->spec(d->index());
}


kRange KvDataOperator::inputRange_(kIndex axis) const
{
	auto d = kPrivate::first_non_null(inputs_);
	if (d == nullptr)
		return kRange{ 0, 0 };

	auto prov = std::dynamic_pointer_cast<KvDataProvider>(d->parent().lock());
	assert(prov);
	return prov->range(d->index(), axis);
}


kReal KvDataOperator::inputStep_(kIndex axis) const
{
	auto d = kPrivate::first_non_null(inputs_);
	if (d == nullptr)
		return 0;

	auto prov = std::dynamic_pointer_cast<KvDataProvider>(d->parent().lock());
	assert(prov);
	return prov->step(d->index(), axis);
}


kIndex KvDataOperator::inputSize_(kIndex axis) const
{
	auto d = kPrivate::first_non_null(inputs_);
	if (d == nullptr)
		return 0;

	auto prov = std::dynamic_pointer_cast<KvDataProvider>(d->parent().lock());
	assert(prov);
	return prov->size(d->index(), axis);
}
