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
	using super_ = KvData;
	using value_type = kReal;

	KtSampled() : samp_() {}


	// 实现基类接口
	kIndex dim() const override {
		return DIM;
	}

	kIndex count() const override {
		kIndex c(1);
		for (kIndex i = 0; i < dim(); i++)
			c *= samp_[i].count();
		return c;
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
		return samp_[axis].count();
	}

	kRange range(kIndex axis) const override {
		assert(axis >= 0 && axis <= dim());
		return axis < dim() ? samp_[axis] : valueRange();
	}

	value_type step(kIndex axis) const override {
		assert(axis >= 0 && axis < dim());
		return samp_[axis].dx();
	}

	// make compiler happy
	//kReal value(kIndex[], kIndex) const override {
	//	assert(false);
	//	return 0;
	//}

	std::vector<kReal> point(kIndex idx[], kIndex channel) const override {
		std::vector<kReal> pt(DIM + 1);
		for (kIndex i = 0; i < DIM; i++)
			pt[i] = indexToValue(i, idx[i]);
		pt[DIM] = value(idx, channel);
		return pt;
	}

	void resize(kIndex shape[]) override {
		for (kIndex i = 0; i < DIM; i++)
			samp_[i].resetn(shape[i]);
	}

	// 调整第axis轴的采样参数
	void reset(kIndex axis, value_type low, value_type step, value_type x0_ref = 0) override {
		assert(axis >= 0 && axis < dim());
		samp_[axis].resetn(size(axis), step, x0_ref);
		samp_[axis].shiftLeftTo(low);
	}

	void reset(kIndex axis, const KtSampling<value_type>& samp) {
		assert(axis >= 0 && axis < dim());
		assert(samp.count() == count());
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

	// 第channel通道的最大最小值
	virtual kRange valueRange(kIndex channel) const {
		if (count() == 0) return { 0, 0 };

		value_type vmin = std::numeric_limits<value_type>::max();
		value_type vmax = std::numeric_limits<value_type>::lowest();

		kIndex idx[DIM] = { 0 };
		for(kIndex i = 0; i < count(); i++) {
			auto val = value(idx, channel);
			if (std::isnan<value_type>(val))
				continue;

			vmin = std::min(vmin, val);
			vmax = std::max(vmax, val);

			nextIndex_(idx);
		}

		return { vmin, vmax };
	}


	// 所有通道的最大最小值
	virtual kRange valueRange() const {
		auto r = valueRange(0);
		for (kIndex c = 1; c < channels(); c++) {
			auto rc = valueRange(c);
			if (rc.low() < r.low())
				r.resetLow(rc.low());
			if (rc.high() > r.high())
				r.resetHigh(rc.high());
		}

		return r;
	}

protected:
	void nextIndex_(kIndex idx[]) const {
		for (kIndex i = 0; i < DIM; i++) {
			if (++idx[i] < size(i))
				break;

			idx[i] = 0; // 进位
		}
	}


protected:
	std::array<KtSampling<value_type>, DIM> samp_; // 各维度的采样参数
};
