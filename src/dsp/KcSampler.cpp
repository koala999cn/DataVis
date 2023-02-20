#include "KcSampler.h"
#include <assert.h>
#include "KtSampling.h"
#include "KvContinued.h"


KcSampler::KcSampler(std::shared_ptr<const KvData> data)
	: internal_(data) 
{
	assert(data);

	x0refs_.resize(dim(), 0);

	// 初始化steps_
	steps_.resize(dim());
	if (data->isContinued()) {
		auto c = std::pow(1024, 1./dim());
		for (unsigned i = 0; i < dim(); i++) {
			KtSampling<kReal> samp;
			samp.resetn(c, data->range(i).low(), data->range(i).high(), x0refs_[i]);
			steps_[i] = samp.dx();
		}
	}
	else {
		auto disc = std::dynamic_pointer_cast<const KvDiscreted>(data);
		assert(disc);
		for (unsigned i = 0; i < dim(); i++) {
			auto dx = disc->step(i);
			steps_[i] = (dx == KvDiscreted::k_nonuniform_step) ? 1 : dx;
		}
	}
}


kIndex KcSampler::dim() const
{
	return internal_->dim();
}


kIndex KcSampler::channels() const
{
	return internal_->channels();
}


kRange KcSampler::range(kIndex axis) const
{
	assert(axis <= dim());
	if (internal_->isContinued()) 
		return internal_->range(axis);

	/// 处理离散数据

	if (axis == dim()) // 值域
		return internal_->valueRange();

	auto disc = std::dynamic_pointer_cast<const KvDiscreted>(internal_);
	assert(disc);
	KtSampling<kReal> samp;
	samp.resetn(disc->size(axis), steps_[axis], x0refs_[axis]);
	return { samp.low(), samp.high() };
}


void KcSampler::resize(kIndex shape[], kIndex channels)
{
	assert(false);
}


kIndex KcSampler::size(kIndex axis) const
{
	assert(axis < dim());

	if (internal_->isContinued()) {
		KtSampling<kReal> samp;
		samp.reset(range(axis).low(), range(axis).high(), steps_[axis], x0refs_[axis]);
		return samp.size();
	}

	auto disc = std::dynamic_pointer_cast<const KvDiscreted>(internal_);
	assert(disc);
	return disc->size(axis);
}


kReal KcSampler::step(kIndex axis) const
{
	assert(axis < dim());
	return steps_[axis];
}


void KcSampler::clear()
{
	assert(false);
}


kReal KcSampler::xToIndex(kReal x) const
{
	assert(false);
	return -1;
}


void KcSampler::reset(kIndex axis, kReal low, kReal step, kReal x0_ref)
{
	assert(axis < dim());
	assert(KuMath::isDefined(low) &&KuMath::isDefined(step));

	if (internal_->isContinued()) {
		// range.low为-inf时，该断言不成立
		//assert(low == internal_->range(axis).low());
	}
	else {
		assert(low == 0);
	}

	steps_[axis] = step;
	x0refs_[axis] = x0_ref;
}


kReal KcSampler::value(kIndex idx[], kIndex channel) const
{
	if (internal_->isContinued()) {
		auto cont = std::dynamic_pointer_cast<const KvContinued>(internal_);
		
		std::vector<kReal> pt(dim());
		for (kIndex i = 0; i < dim(); i++) {
			KtSampling<kReal> samp;
			samp.reset(range(i).low(), range(i).high(), steps_[i], x0refs_[i]);
			pt[i] = samp.indexToX(idx[i]);
		}

		return cont->value(pt.data(), channel);
	}

	auto samp = std::dynamic_pointer_cast<const KvSampled>(internal_);
	assert(samp);
	return samp->value(idx, channel);
}


std::vector<kReal> KcSampler::point(kIndex idx[], kIndex channel) const
{
	std::vector<kReal> pt(dim() + 1);
	for (kIndex i = 0; i < dim(); i++) {
		KtSampling<kReal> samp;
		samp.reset(range(i).low(), range(i).high(), steps_[i], x0refs_[i]);
		pt[i] = samp.indexToX(idx[i]);
	}

	if (internal_->isContinued()) {
		auto cont = std::dynamic_pointer_cast<const KvContinued>(internal_);
		pt.back() = cont->value(pt.data(), channel);
	}
	else {
		auto samp = std::dynamic_pointer_cast<const KvSampled>(internal_);
		assert(samp);
		pt.back() = samp->value(idx, channel);
	}

	return pt;
}


void KcSampler::setData(std::shared_ptr<const KvData> d)
{
	internal_ = d;
	steps_.resize(d->dim(), steps_[0]);
	x0refs_.resize(d->dim(), x0refs_[0]);
}
