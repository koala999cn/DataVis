#pragma once
#include <memory>
#include "KtSampled.h"
#include "KvContinued.h"


// 对连续数据的采样器

template<int DIM>
class KtSampler : public KtSampled<DIM>
{
public:
	using super_ = KtSampled<DIM>;
	
	KtSampler(std::shared_ptr<KvContinued> cont)
		: internal_(cont) {
		for (kIndex i = 0; i < cont->dim(); i++)
			super_::samp_[i].resetn(100, cont->range(i).low(), cont->range(i).high(), 0.5);
	}


	kIndex channels() const final {
		return internal_->channels();
	}

	kReal value(kIndex idx[], kIndex channel) const final {
		kReal pt[DIM];
		for (kIndex i = 0; i < DIM; i++)
			pt[i] = super_::indexToValue(i, idx[i]);

		return internal_->value(pt, channel);
	}

	void reset(kIndex axis, kIndex nx, kReal xmin, kReal xmax, kReal x0_rel_offset = 0) {
		super_::samp_[axis].resetn(nx, xmin, xmax, x0_rel_offset);
	}

private:
	const std::shared_ptr<KvContinued> internal_;
};