#pragma once
#include "KvData.h"
#include "KtuMath.h"
#include <array>


template<typename FUN, unsigned DIM>
class KtContinued : public KvData
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
		return axis < DIM ? range_[axis] : valueRange();
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


	void setRange(kIndex axis, kReal low, kReal high) {
		range_[axis] = { low, high };
	}

	// 第channel通道的最大最小值（使用二分算法粗略计算）
	virtual kRange valueRange(kIndex channel) const {
		if (count() == 0) return { 0, 0 };

		assert(DIM == 1); // TODO: 暂时实现一维算法

		kReal omin = std::numeric_limits<kReal>::max();
		kReal omax = std::numeric_limits<kReal>::lowest();

		kReal low = range(0).low();
		if (std::isinf(low)) low = -1e8;
		kReal high = range(0).high();
		if (std::isinf(high)) high = 1e8;
		kReal dx((high - low)/2);
		kReal tol(0.001); // 百分之一的误差
		int minIter(10), maxIter(16), numIter(0);

		while (true) {
			kReal nmin = std::numeric_limits<kReal>::max();
			kReal nmax = std::numeric_limits<kReal>::lowest();
			for (kReal x = low; x < high; x += dx) {
				auto val = value(&x, channel);
				if (val > nmax)
					nmax = val;
				if (val < nmin)
					nmin = val;
			}

			if ((KtuMath<kReal>::almostEqualRel(omin, nmin, tol) &&
				KtuMath<kReal>::almostEqualRel(omax, nmax, tol) &&
				numIter > minIter) || numIter > maxIter) 
				return { nmin, nmax };


			omin = nmin, omax = nmax, dx *= 0.5;
			++numIter;
		}

		return { omin, omax };
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
	std::vector<FUN> fun_; // size等于通道数
	std::array<kRange, DIM> range_; // size等于dim数
};
