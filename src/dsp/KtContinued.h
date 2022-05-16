#pragma once
#include "KvContinued.h"
#include <array>


template<typename FUN, unsigned DIM>
class KtContinued : public KvContinued
{
public:
	KtContinued(FUN fun, kReal low, kReal high) 
		: fun_{ fun }, range_({ kRange{ low, high } }) {} // 单通道

	KtContinued(FUN fun) 
		: KtContinued(fun, 0, 1) {}

	KtContinued(FUN fun1, kReal low1, kReal high1, FUN fun2, kReal low2, kReal high2)
		: fun_{ fun1, fun2 }, range_({ kRange{ low1, high1 }, kRange{ low2, high2 } }) {} // 双通道

	KtContinued(FUN fun1, FUN fun2)
		: KtContinued(fun1, 0, 1, fun2, 0, 1) {}

	KtContinued(FUN funs[], unsigned channels, const std::array<kRange, DIM>& ranges)
		: fun_(funs, funs + channels), range_(ranges) {} // 多通道


	constexpr kIndex dim() const override {
		return DIM;
	}

	kIndex channels() const override { 
		return static_cast<kIndex>(fun_.size());
	}

	kRange range(kIndex axis) const override {
		return axis < DIM ? range_[axis] : valueRange();
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


	void setRange(kIndex axis, kReal low, kReal high) {
		range_[axis] = { low, high };
	}

private:
	std::vector<FUN> fun_; // size等于通道数
	std::array<kRange, DIM> range_; // size等于dim数
};
