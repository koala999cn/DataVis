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


KcFramingOp::kPropertySet KcFramingOp::propertySet() const
{
	kPropertySet ps;

	return ps;
}


void KcFramingOp::onPropertyChanged(int id, const QVariant& newVal)
{

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
