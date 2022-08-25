#include "KcOpFraming.h"
#include <assert.h>
#include "KcSampled1d.h"
#include "KcSampled2d.h"
#include "QtAppEventHub.h"


KcOpFraming::KcOpFraming(KvDataProvider* prov) 
	: KvDataOperator("framing", prov)
	, dx_(prov->step(0))
	, frameTime_(512 * dx_)
    , shiftTime_(frameTime_ / 2)
{
	assert(prov->dim() == 1);
	assert(prov->isSampled());
	preRender_();
}


namespace kPrivate
{
	enum KeFramingPropertyId
	{
		k_length,
		k_frame_size,
		k_shift,
		k_shift_size
	};
}

KcOpFraming::kPropertySet KcOpFraming::propertySet() const
{
	kPropertySet ps;

	auto prov = dynamic_cast<KvDataProvider*>(parent());
	KpProperty prop;
	prop.id = kPrivate::k_length;
	prop.name = tr(u8"length");
	prop.disp = tr(u8"Frame Length");
	prop.desc = tr(u8"frame duration in second");
	prop.val = frameTime_;
	prop.minVal = prov->step(0);
	prop.maxVal = std::numeric_limits<kReal>::max();
	ps.push_back(prop);

	prop.id = kPrivate::k_frame_size;
	prop.name = tr(u8"frame_size");
	prop.disp = tr(u8"Frame Size");
	prop.desc = tr(u8"number of samples per frame");
	prop.val = int(frameSize());
	prop.flag = k_readonly;
	ps.push_back(prop);

	prop.id = kPrivate::k_shift;
	prop.name = tr(u8"shift");
	prop.disp = tr(u8"Frame Shift");
	prop.desc = tr(u8"frame by frame shift in second");
	prop.val = shiftTime_;
	prop.minVal = prov->step(0);
	prop.maxVal = std::numeric_limits<kReal>::max();
	prop.flag = 0;
	ps.push_back(prop);

	prop.id = kPrivate::k_shift_size;
	prop.name = tr(u8"shift_size");
	prop.disp = tr(u8"Shift Size");
	prop.desc = tr(u8"number of samples once shift");
	prop.val = int(shiftSize());
	prop.flag = k_readonly;
	ps.push_back(prop);

	return ps;
}


kIndex KcOpFraming::dim() const
{
	return KvDataOperator::dim() + 1; // 比父数据多一个维度
}


void KcOpFraming::setPropertyImpl_(int id, const QVariant& newVal)
{
	switch (id) {
	case kPrivate::k_length:
		frameTime_ = newVal.toFloat();
		emit kAppEventHub->objectPropertyChanged(this, 
			kPrivate::k_frame_size, int(frameSize())); // 同步属性页的k_frame_size值
		break;

	case kPrivate::k_shift:
		shiftTime_ = newVal.toFloat();
		emit kAppEventHub->objectPropertyChanged(this,
			kPrivate::k_shift_size, int(shiftSize())); // // 同步属性页的k_shift_size值
		break;
	};
}


kIndex KcOpFraming::frameSize() const
{
	KtSampling<kReal> samp;
	samp.reset(0, frameTime_, dx_, 0);
	return samp.size();
}


kIndex KcOpFraming::shiftSize() const
{
	KtSampling<kReal> samp;
	samp.reset(0, shiftTime_, dx_, 0);
	return samp.size();
}


void KcOpFraming::preRender_()
{
	auto prov = dynamic_cast<KvDataProvider*>(parent());
	if (!framing_ || dx_ != prov->step(0) ||
		framing_->channels() != prov->channels() ||
		framing_->size() != frameSize() ||
		framing_->shift() != shiftSize()) {
		dx_ = prov->step(0);
		framing_ = std::make_unique<KtFraming<kReal>>(frameSize(), prov->channels(), shiftSize());
	}
}


kRange KcOpFraming::range(kIndex axis) const
{
	if (axis == 1)
		return { 0, frameTime_ };
	else if(axis == 0)
	    return KvDataOperator::range(0);
	
	return KvDataOperator::range(axis - 1);
}


kReal KcOpFraming::step(kIndex axis) const
{
	if (axis == 0)
		return shiftTime_;

	return KvDataOperator::step(axis - 1);
}


kIndex KcOpFraming::size(kIndex axis) const
{
	if (axis == 1)
		return frameSize();
	else if (axis == 0)
		return framing_->outFrames(KvDataOperator::size(0), false);

	return KvDataOperator::size(axis - 1);
}


std::shared_ptr<KvData> KcOpFraming::processImpl_(std::shared_ptr<KvData> data)
{
	assert(data && data->dim() == 1);
	assert(framing_);
	
	auto data1d = std::dynamic_pointer_cast<KcSampled1d>(data);
	assert(dx_ == data1d->step(0));

	auto res = std::make_shared<KcSampled2d>();
	auto frameNum = framing_->outFrames(data1d->size(), true);
	auto frameSize = this->frameSize();
	kReal x0 = data1d->sampling(0).low();
	x0 -= framing_->buffered() * dx_;
	res->resize(frameNum, frameSize, framing_->channels());
	res->reset(0, x0 + frameTime_ / 2, shiftTime_);
	res->reset(1, x0, dx_);

	auto first = data1d->data();
	auto last = first + data1d->size();
	kIndex idx(0);
	framing_->apply(first, last, [&res, &idx](const kReal* data) {
		std::copy(data, data + res->size(1), res->row(idx++));
		});

	return res;
}
