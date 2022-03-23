#include "KcFramingOp.h"
#include <assert.h>
#include "KcSampled1d.h"
#include "KcSampled2d.h"
#include "KgFraming.h"


KcFramingOp::KcFramingOp(KvDataProvider* prov) 
	: KvDataOperator("framing", prov)
{
	assert(prov->dim() == 1);
	assert(prov->step(0) != KvData::k_unknown_step && prov->step(0) != KvData::k_nonuniform_step);
	d_ptr_ = new KgFraming(1 / prov->step(0));
}


KcFramingOp::~KcFramingOp()
{
	delete (KgFraming*)d_ptr_;
}

namespace kPrivate
{
	enum KeFramingPropertyId
	{
		k_length,
		k_shift
	};
}

KcFramingOp::kPropertySet KcFramingOp::propertySet() const
{
	kPropertySet ps;

	auto prov = dynamic_cast<KvDataProvider*>(parent());
	KpProperty prop;
	prop.id = kPrivate::k_length;
	prop.name = tr(u8"length");
	prop.disp = tr(u8"Frame Length");
	prop.desc = tr(u8"frame duration in second");
	prop.val = float(((KgFraming*)d_ptr_)->length());
	prop.minVal = float(prov->step(0));
	prop.maxVal = std::numeric_limits<float>::max();

	ps.push_back(prop);


	prop.id = kPrivate::k_shift;
	prop.name = tr(u8"shift");
	prop.disp = tr(u8"Frame Shift");
	prop.desc = tr(u8"frame by frame shift in second");
	prop.val = float(((KgFraming*)d_ptr_)->shift());
	prop.minVal = float(prov->step(0));
	prop.maxVal = std::numeric_limits<float>::max();
	ps.push_back(prop);

	return ps;
}


void KcFramingOp::onPropertyChanged(int id, const QVariant& newVal)
{
	switch (id) {
	case kPrivate::k_length:
		((KgFraming*)d_ptr_)->setLength(newVal.toFloat());
		break;

	case kPrivate::k_shift:
		((KgFraming*)d_ptr_)->setShift(newVal.toFloat());
		break;
	};
}


kRange KcFramingOp::range(kIndex axis) const
{
	if (axis == 1)
		return { 0, ((KgFraming*)d_ptr_)->length() };
	else if(axis == 0)
	    return KvDataOperator::range(0);
	else 
		return KvDataOperator::range(1);
}


kReal KcFramingOp::step(kIndex axis) const
{
	if (axis == 0)
		return ((KgFraming*)d_ptr_)->shift();

	return KvDataOperator::step(axis - 1);
}


std::shared_ptr<KvData> KcFramingOp::processImpl_(std::shared_ptr<KvData> data)
{
	assert(data && data->dim() == 1);

	auto framObj = (KgFraming*)d_ptr_;
	assert(framObj);
	
	auto data1d = std::dynamic_pointer_cast<KcSampled1d>(data);

	auto res = std::make_shared<KcSampled2d>();

	if (data->empty())
		framObj->flush(*res);
	else
		framObj->process(*data1d, *res);

	return res;
}
