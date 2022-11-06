#include "KvPlottable.h"
#include "KvData.h"


void KvPlottable::setData(data_ptr d) 
{
	data_ = d;

	if (d->isContinued() && d->dim() != sampCount_.size())
		sampCount_.assign(d->dim(), std::pow(1000., 1. / d->dim()));
}


bool KvPlottable::empty() const 
{
	return !data_ || data_->size() == 0;
}


KvPlottable::aabb_type KvPlottable::boundingBox() const
{
	if (empty())
		return aabb_type(); // null aabb

	point3 lower, upper;

	auto r0 = data_->range(0);
	auto r1 = data_->range(1);

	lower.x() = r0.low(), upper.x() = r0.high();
	lower.y() = r1.low(), upper.y() = r1.high();

	if (data_->dim() > 1) {
		auto r2 = data_->range(2);
		lower.z() = r2.low(), upper.z() = r2.high();
	}
	else {
		lower.z() = upper.z() = 0;
	}

	return { lower, upper };
}