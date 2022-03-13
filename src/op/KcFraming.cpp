#include "KcFraming.h"
#include <assert.h>
#include "KcSampled1d.h"


KcFraming::KcFraming(KvDataProvider* prov) 
	: KvDataOperator("framing", prov)
	, length_(0.05) // 50ms
	, shift_(0.025) // 25ms
{
	assert(prov->dim() == 1);
}


KcFraming::kPropertySet KcFraming::propertySet() const
{
	kPropertySet ps;

	return ps;
}


void KcFraming::onPropertyChanged(int id, const QVariant& newVal)
{

}


kRange KcFraming::range(kIndex axis) const
{
	if (axis == 0)
		return { 0, length_ };

	return KvDataOperator::range(axis);
}


kReal KcFraming::step(kIndex axis) const
{
	if (axis == 0)
		return shift_;

	return KvDataOperator::step(axis);
}


std::shared_ptr<KvData> KcFraming::processImpl_(std::shared_ptr<KvData> data)
{
	assert(data && data->dim() == 1);
	auto buf = dynamic_cast<KcSampled1d*>(buf_.get());

	std::shared_ptr<KcSampled1d> res;
	auto frameLen = length_;
	auto shift = shift_;

	if (data->empty()) { // flush. 返回缓存的数据，并清空缓存
		if (buf && !buf->empty()) {
			res->copy(*buf);
			buf->clear();

			// 给res后面补0
			auto nx = res->sampling().countLength(frameLen);
			res->resize(nx, 0);
		}
	}
	else {
		auto data1d = std::dynamic_pointer_cast<KvData1d>(data);
		assert(data1d);

		if (buf == 0) { // 第一次接收数据，初始化
			auto x0_ref = (data1d->value(0).x - data1d->range(0).low()) / data->step(0);
			buf = new KcSampled1d(data1d->step(0), x0_ref, data1d->channels());
			buf_.reset(buf);
		}

		buf->append(*data1d);

		auto samplesPerFrame = buf->sampling().countLength(frameLen);
		while (buf->count() >= samplesPerFrame) {
			res->copy(*buf, 0, samplesPerFrame);
			buf->cutFront(shift);

			// TODO: 
		}
	}

	return res;
}
