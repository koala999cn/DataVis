#pragma once
#include <array>
#include <assert.h>
#include "kDsp.h" // for kReal
#include "KtSampling.h"
#include "KvData.h"


// 采样数据基类接口

// @BASE: 须是KvData的子类
// @DIM: DIM=1时创建2维array, DIM=2时创建3维array, ...
template<typename BASE, int DIM>
class KtSampled : public BASE
{
public:
	using value_type = kReal;

	KtSampled() = default;

	// 实现基类接口
	//constexpr kIndex dim() const override {
	//	return DIM;
	//}

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

	kIndex length(kIndex axis) const override {
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


	// 新的接口
	virtual value_type value(std::array<kIndex, DIM> idx, kIndex channel) const = 0;


	// 帮助函数

	// 调整第axis轴的采样参数
	void reset(kIndex axis, value_type low, value_type step, value_type x0_ref = 0) {
		assert(axis >= 0 && axis < dim());
		samp_[axis].resetn(length(axis), step, x0_ref);
		samp_[axis].shiftLeftTo(low);
	}

	void reset(kIndex axis, const KtSampling<value_type>& samp) {
		assert(axis >= 0 && axis < dim());
		samp_[axis] = samp;
	}

	kReal indexToValue(kIndex axis, kIndex idx) const {
		assert(axis >= 0 && axis < dim());
		return samp_[axis].indexToX(idx);
	}

	auto point(std::array<kIndex, DIM> idx, kIndex channel) const {
		std::array<kIndex, DIM + 1> pt;
		for (kIndex i = 0; i < DIM; i++)
			pt[i] = indexToValue(idx[i]);
		pt[DIM] = value(idx, channel);
		return pt;
	}

	void nextIndex(std::array<kIndex, DIM>& idx) const {
		for (kIndex i = 0; i < DIM; i++) {
			if (++idx[i] < length(i))
				break;

			idx[i] = 0; // 进位
		}
	}

	// 第channel通道的最大最小值
	virtual kRange valueRange(kIndex channel) const {
		if (count() == 0) return { 0, 0 };

		value_type vmin = std::numeric_limits<value_type>::max();
		value_type vmax = std::numeric_limits<value_type>::lowest();

		std::array<kIndex, DIM> idx; idx.fill(0);
		for(kIndex i = 0; i < count(); i++) {
			auto val = value(idx, channel);
			if (std::isnan<value_type>(val))
				continue;

			vmin = std::min(vmin, val);
			vmax = std::max(vmax, val);

			nextIndex(idx);
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

private:
	std::array<KtSampling<value_type>, DIM> samp_; // 各维度的采样参数
};
