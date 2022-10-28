#pragma once
#include "KvContinued.h"
#include <array>
#include <assert.h>


template<typename FUN, unsigned DIM>
class KtContinued : public KvContinued
{
public:
	static inline const auto k_unknown_range = kRange{ 0, 0 };

	KtContinued(FUN fun, kReal low, kReal high) // 单通道
		: fun_{ fun } {
		range_.fill(kRange{ low, high });
		range_[DIM] = k_unknown_range;
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

		if (axis < DIM)
			return range_[axis];
		
		return range_[axis] == k_unknown_range ? valueRange() : range_[axis];
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
	}

private:
	std::vector<FUN> fun_; // size等于通道数
	std::array<kRange, DIM + 1> range_; // range_[DIM]表示valueRange. NAN表示未设置
};
