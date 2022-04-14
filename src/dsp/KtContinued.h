#pragma once
#include "KvData.h"


template<typename FUN, unsigned DIM>
class KtContinued : public KvData
{
public:
	KtContinued(FUN fun) : fun_{ fun }, range_{} {} // 单通道
	KtContinued(FUN fun1, FUN fun2) : fun_{ fun1, fun2 }, range_{} {} // 双通道
	KtContinued(FUN funs[], unsigned channels) : fun_(funs, funs + channels), range_{} {} // 多通道

	constexpr kIndex dim() const override {
		return DIM;
	}

	kIndex count() const override {
		return k_inf_count;
	}

	kIndex channels() const override { 
		return static_cast<kIndex>(fun_.size());
	}

	void clear() override {}

	bool empty() const override { 
		return range_.empty(); 
	}

	kIndex length(kIndex axis) const override {
		return count();
	}

	kRange range(kIndex axis) const override {
		return range_;
	}

	kReal step(kIndex axis) const override {
		return 0;
	}

	kReal value(kIndex idx[], kIndex channel) const override {
		assert(false);
		return 0; // TODO:
	}

	std::vector<kReal> point(kIndex idx[], kIndex channel) const override {
		assert(false);
		return { 0 }; // TODO
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


	void setRange(kReal low, kReal high) {
		range_ = { low, high };
	}

private:
	std::vector<FUN> fun_;
	KtInterval<kReal> range_;
};
