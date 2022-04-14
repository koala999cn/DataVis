#pragma once
#include <memory>
#include "KtSampled.h"


// 对连续数据的采样器

template<int DIM>
class KtSampler : public KtSampled<DIM>
{
public:
	KtSampler(std::shared_ptr<KvData>& cont) 
		: internal_(cont) {}


	kIndex channels() const final {
		return internal_->channels();
	}

	kReal value(kIndex idx[], kIndex channel) const final {
		kReal pt[DIM];
		for (kIndex i = 0; i < DIM; i++)
			pt[i] = indexToValue(i, idx[i]);

		return value(pt, channel);
	}


	kReal value(kReal pt[], kIndex channel) const final {
		return internal_->value(pt, channel);
	}

private:
	const std::shared_ptr<KvData> internal_;
};