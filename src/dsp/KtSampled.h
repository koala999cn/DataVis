#pragma once
#include <array>
#include <assert.h>
#include "kDsp.h" // for kReal
#include "KtSampling.h"
#include "KvSampled.h"


// 采样数据基类接口

// @DIM: DIM=1时创建2维采样数据, DIM=2时创建3维采样数据, ...
template<int DIM>
class KtSampled : public KvSampled
{
public:
	using super_ = KvSampled;
	using value_type = kReal;

	KtSampled() : samp_() {}


	// 实现基类接口
	kIndex dim() const override {
		return DIM;
	}

	void clear() override {
		for (kIndex i = 0; i < dim(); i++)
			samp_[i].clear();
	}

	bool empty() const override {
		return samp_[0].empty();
	}

	kIndex size(kIndex axis) const override {
		assert(axis >= 0 && axis < dim());
		return samp_[axis].size();
	}

	kRange range(kIndex axis) const override {
		assert(axis >= 0 && axis <= dim());
		return axis < dim() ? samp_[axis] : valueRange();
	}

	value_type step(kIndex axis) const override {
		assert(axis >= 0 && axis < dim());
		return samp_[axis].dx();
	}

	std::vector<kReal> point(kIndex idx[], kIndex channel) const override {
		std::vector<kReal> pt(DIM + 1);
		for (kIndex i = 0; i < DIM; i++)
			pt[i] = indexToValue(i, idx[i]);
		pt[DIM] = value(idx, channel);
		return pt;
	}

	kReal xToIndex(kReal x) const override {
		return samp_[0].xToIndex(x);
	}

	void resize(kIndex shape[]) override {
		for (kIndex i = 0; i < DIM; i++)
			samp_[i].resetn(shape[i]);
	}

	// 调整第axis轴的采样参数
	void reset(kIndex axis, value_type low, value_type step, value_type x0_ref = 0) override {
		assert(axis >= 0 && axis < dim());
		samp_[axis].shiftLeftTo(low);
		samp_[axis].resetn(size(axis), step, x0_ref);
		assert(size(axis) == samp_[axis].size());
	}

	void reset(kIndex axis, const KtSampling<value_type>& samp) {
		assert(axis >= 0 && axis < dim());
		assert(samp.size() == super_::size());
		samp_[axis] = samp;
	}


	// 帮助函数

	const KtSampling<value_type>& sampling(kIndex axis) const {
		return samp_[axis];
	}

	kReal indexToValue(kIndex axis, kIndex idx) const {
		assert(axis >= 0 && axis < dim());
		return samp_[axis].indexToX(idx);
	}

protected:
	std::array<KtSampling<value_type>, DIM> samp_; // 各维度的采样参数
};
