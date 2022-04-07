#include "KcOpFraming.h"
#include <assert.h>
#include "KcSampled1d.h"
#include "KcSampled2d.h"
#include "KgFraming.h"
#include "QtAppEventHub.h"


KcOpFraming::KcOpFraming(KvDataProvider* prov) 
	: KvDataOperator("framing", prov)
	, dx_(prov->step(0))
	, channels_(prov->channels())
{
	assert(prov->dim() == 1);
	assert(dx_ != KvData::k_unknown_step && dx_ != KvData::k_nonuniform_step);
	syncParent();
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
	prop.val = float(framing_->length());
	prop.minVal = float(prov->step(0));
	prop.maxVal = std::numeric_limits<float>::max();
	ps.push_back(prop);

	prop.id = kPrivate::k_frame_size;
	prop.name = tr(u8"frame_size");
	prop.disp = tr(u8"Frame Size");
	prop.desc = tr(u8"number of samples per frame");
	prop.val = framing_->frameSize();
	prop.flag = k_readonly;
	ps.push_back(prop);

	prop.id = kPrivate::k_shift;
	prop.name = tr(u8"shift");
	prop.disp = tr(u8"Frame Shift");
	prop.desc = tr(u8"frame by frame shift in second");
	prop.val = float(framing_->shift());
	prop.minVal = float(prov->step(0));
	prop.maxVal = std::numeric_limits<float>::max();
	prop.flag = 0;
	ps.push_back(prop);

	prop.id = kPrivate::k_shift_size;
	prop.name = tr(u8"shift_size");
	prop.disp = tr(u8"Shift Size");
	prop.desc = tr(u8"number of samples once shift");
	prop.val = framing_->shiftSize();
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
		framing_->setLength(newVal.toFloat());
		emit kAppEventHub->objectPropertyChanged(this, 
			kPrivate::k_frame_size, framing_->frameSize()); // 同步属性页的k_frame_size值
		break;

	case kPrivate::k_shift:
		framing_->setShift(newVal.toFloat());
		emit kAppEventHub->objectPropertyChanged(this,
			kPrivate::k_shift_size, framing_->shiftSize()); // // 同步属性页的k_shift_size值
		break;
	};
}


void KcOpFraming::syncParent()
{
	auto prov = dynamic_cast<KvDataProvider*>(parent());
	if (!framing_ || dx_ != prov->step(0) || channels_ != prov->channels()) {
		dx_ = prov->step(0);
		channels_ = prov->channels();
		framing_ = std::make_unique<KgFraming>(dx_, channels_);
	}
}


kRange KcOpFraming::range(kIndex axis) const
{
	if (axis == 1)
		return { 0, framing_->length() };
	else if(axis == 0)
	    return KvDataOperator::range(0);
	
	return KvDataOperator::range(axis - 1);
}


kReal KcOpFraming::step(kIndex axis) const
{
	if (axis == 0)
		return framing_->shift();

	return KvDataOperator::step(axis - 1);
}


kIndex KcOpFraming::length(kIndex axis) const
{
	if (axis == 1)
		return framing_->frameSize();
	else if (axis == 0)
		return framing_->numFrames(KvDataOperator::length(0));

	return KvDataOperator::length(axis - 1);
}


std::shared_ptr<KvData> KcOpFraming::processImpl_(std::shared_ptr<KvData> data)
{
	assert(data && data->dim() == 1);
	assert(framing_);
	
	auto data1d = std::dynamic_pointer_cast<KcSampled1d>(data);

	auto res = std::make_shared<KcSampled2d>();

	if (data->empty())
		framing_->flush(*res);
	else
		framing_->process(*data1d, *res);

	return res;
}
