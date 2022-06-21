#pragma once
#include <memory>
#include "KtSampled.h"
#include "KvContinued.h"


// 对连续数据的采样器

template<int DIM>
class KtSampler : public KtSampled<DIM>
{
public:
	KtSampler(std::shared_ptr<KvContinued>& cont)
		: internal_(cont) {
		for (kIndex i = 0; i < cont->dim(); i++)
			samp_[i].resetn(100, cont->range(i).low(), cont->range(i).high(), 0.5); // TODO: 100
	}


	kIndex channels() const final {
		return internal_->channels();
	}

	kReal value(kIndex idx[], kIndex channel) const final {
		kReal pt[DIM];
		for (kIndex i = 0; i < DIM; i++)
			pt[i] = indexToValue(i, idx[i]);

		return internal_->value(pt, channel);
	}


private:
	const std::shared_ptr<KvContinued> internal_;
};