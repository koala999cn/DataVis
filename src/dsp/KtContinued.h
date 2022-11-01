#pragma once
#include "KvContinued.h"
#include <array>
#include <assert.h>


template<typename FUN, unsigned DIM>
class KtContinued : public KvContinued
{
public:
	KtContinued(FUN fun, kReal low, kReal high) // 单通道
		: fun_{ fun } {
		range_.fill(kRange{ low, high });
	} 

	KtContinued(FUN fun) 
		: KtContinued(fun, 0, 1) {}

	constexpr kIndex dim() const override {
		return DIM;
	}

	kIndex channels() const override { 
		return static_cast<kIndex>(fun_.size());
	}

	kRange range(kIndex axis) const override {
		assert(axis <= DIM);

		if (axis == DIM) {
			if (valueRangeStatus_ == k_unknown ||
				valueRangeStatus_ == k_expired) {
				range_[axis] = valueRange();
				valueRangeStatus_ = k_esimated;
			}
		}

		return range_[axis];
	}


	kReal value(kReal pt[], kIndex channel) const override {
		if constexpr (DIM == 1)
			return fun_[channel](pt[0]);
		else if constexpr (DIM == 2)
			return fun_[channel](pt[0], pt[1]);
		else if constexpr (DIM == 3)
			return fun_[channel](pt[0], pt[1], pt[2]);
		else
			return fun_[channel](pt); 
	}


	void setRange(kIndex axis, kReal low, kReal high) override {
		range_[axis] = { low, high };
		if (axis == DIM)
			valueRangeStatus_ = k_specified;
		else if (valueRangeStatus_ == k_esimated)
			valueRangeStatus_ = k_expired;
	}

private:
	std::vector<FUN> fun_; // size等于通道数
	mutable std::array<kRange, DIM + 1> range_; // range_[DIM]表示valueRange.

	enum KeValueRangeStatus
	{
		k_unknown, // 未设置，初始状态
		k_specified, // 已指定，由用户调用setRange设置
		k_esimated, // 估算值，由内部调用基类的valueRange估计
		k_expired // 过期（用户重新设定了区域范围），需要重新估算
	};

	mutable int valueRangeStatus_{ k_unknown };
};
