#include "KvDataOperator.h"
#include "KvDiscreted.h"
#include "KuMath.h"


KvDataOperator::KvDataOperator(const std::string_view& name)
    : super_(name)
	, idata_(inPorts(), nullptr)
	, odata_(outPorts(), nullptr)
    , idataStamps_(inPorts(), 0)
	, odataStamps_(outPorts(), 0)
	, outputExpired_(outPorts(), true)
	, inputs_(inPorts(), nullptr)
{
	
}


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
	else { // value range�����ȴ�prov��ȡ
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
	if (to->parent().lock().get() == this) { // ֻ������������
		assert(to->index() < inputs_.size());
		
		auto prov = std::dynamic_pointer_cast<KvDataProvider>(from->parent().lock());
		if (prov == nullptr  // ֻ����provider�ڵ����
			|| inputs_[to->index()]) // ÿ������˿�ֻ����������
			return false; 

		if (!permitInput(prov->spec(from->index()), to->index()))
			return false; // �����ܵ����ݹ��

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

		idata_[to->index()] = nullptr;

		// �ٶ�����Ӱ���������
		for (auto& o : odata_)
			o = nullptr;
	}
}


void KvDataOperator::onInput(KcPortNode* outPort, unsigned inPort)
{
	assert(outPort && inPort < inputs_.size());
	assert(inputs_[inPort] == outPort);

	auto prov = std::dynamic_pointer_cast<KvDataProvider>(outPort->parent().lock());
	assert(prov);

	if (prov->dataStamp(outPort->index()) > idataStamps_[inPort]){ // �Ƿ��и��µ�����
		idata_[inPort] = prov->fetchData(outPort->index());
		idataStamps_[inPort] = prov->dataStamp(outPort->index());
	}
}


std::shared_ptr<KvData> KvDataOperator::fetchData(kIndex outPort) const
{
	assert(outPort < odata_.size());

	return odata_[outPort];
}


void KvDataOperator::notifyChanged(unsigned outPort)
{
	if (outPort == -1) {
		for (auto& i : odataStamps_)
			i = currentFrameIndex_();
	}
	else {
		odataStamps_[outPort] = currentFrameIndex_();
	}

	super_::notifyChanged(outPort);
}


unsigned KvDataOperator::dataStamp(kIndex outPort) const
{
	return odataStamps_[outPort];
}


bool KvDataOperator::onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins)
{
	if (ins.size() != inPorts())
		return false; // ����δȫ�����ӣ�������

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


void KvDataOperator::setOutputExpired(unsigned outPort)
{
	outputExpired_[outPort] = true;
}


bool KvDataOperator::isOutputExpired(unsigned outPort) const
{
	return outputExpired_[outPort];
}


bool KvDataOperator::isOutputExpired() const
{
	return std::find(outputExpired_.begin(), outputExpired_.end(), true) != outputExpired_.end();
}


bool KvDataOperator::isInputUpdated(unsigned inPort) const
{
	for (auto i : odataStamps_)
		if (idataStamps_[0] > i)
			return true;
	return false;
}


bool KvDataOperator::isInputUpdated() const
{
	for (unsigned i = 0; i < inputs_.size(); i++)
		if (isInputUpdated(i))
			return true;
	return false;
}


void KvDataOperator::output() 
{
	if (isInputUpdated() || (isOutputExpired() && !isDynamic(0))) { // TODO: �˴�ֻ�����0�˿ڵĶ�̬����
		prepareOutput_();
		outputImpl_();
		notifyChanged(); // ����odata��ʱ�����currentFrame
		std::fill(outputExpired_.begin(), outputExpired_.end(), false);
	}
}