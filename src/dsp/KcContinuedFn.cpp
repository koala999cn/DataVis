#include "KcContinuedFn.h"


KcContinuedFn::KcContinuedFn(std::vector<fn_type> fun, unsigned dim)
	: fun_(fun)
{
	range_.resize(++dim, kRange{ 0, 1 });
}


KcContinuedFn::KcContinuedFn(fn_type fun, unsigned dim)
	: fun_{ fun }
{
	range_.resize(++dim, kRange{ 0, 1 });
}


KcContinuedFn::KcContinuedFn(std::function<kReal(kReal)> fun)
{
	fun_.push_back([fun](kReal pt[]) { return fun(pt[0]); });
	range_.resize(2, kRange{ 0, 1 });
}


KcContinuedFn::KcContinuedFn(std::function<kReal(kReal, kReal)> fun)
{
	fun_.push_back([fun](kReal pt[]) { return fun(pt[0], pt[1]); });
	range_.resize(3, kRange{ 0, 1 });
}


kRange KcContinuedFn::range(kIndex axis) const 
{
	assert(axis <= dim());

	if (axis == dim()) {
		if (valueRangeStatus_ == k_unknown ||
			valueRangeStatus_ == k_expired) {
			range_[axis] = valueRange();
			valueRangeStatus_ = k_esimated;
		}
	}

	return range_[axis];
}


kReal KcContinuedFn::value(kReal pt[], kIndex channel) const 
{
	return fun_[channel](pt);
}


void KcContinuedFn::setRange(kIndex axis, kReal low, kReal high) 
{
	assert(axis < range_.size());

	range_[axis] = { low, high };

	if (axis == dim())
		valueRangeStatus_ = k_specified;
	else if (valueRangeStatus_ == k_esimated)
		valueRangeStatus_ = k_expired;
}
