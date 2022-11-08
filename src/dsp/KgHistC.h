#pragma once
#include "KmBins.h"


// TODO: 待实现
// 计数直方统计

// 支持以下类型：
//   count: vi = ci，为观测值的计数或频率
//   countdensity: vi = ci / wi，计数除以bin的宽度
//   cumcount: vi = sum(cj), 1 <= j <= i, 累积计数
//   probability: vi = ci / N，相对概率
//   pdf: vi = ci / (N * wi), 概率密度函数估计
//   cdf: vi = sum(cj/N), 1 <= j <= i, 累积密度函数估计
//

class KgHistC : public KmBins
{
public:
	KgHistC() : KmBins() {}

	void resetLinear(kIndex numBins, kReal low, kReal high) {
		KmBins::resetLinear(numBins, low, high);
		res_.resize(numBins);
		reset();
	}

	void resetLog(kIndex numBins, kReal low, kReal high) {
		KmBins::resetLog(numBins, low, high);
		res_.resize(numBins);
		reset();
	}

	void resetCustom(const std::vector<kReal>& bins) {
		KmBins::resetCustom(bins);
		res_.resize(bins.size());
		reset();
	}

	// 清空累计结果
	void reset();

	// 对data进行累计计数
	void count(const kReal* data, kIndex N);

	// 返回每个bin的计数结果
	const std::vector<unsigned>& result() const {
		return res_;
	}

private:
	std::vector<unsigned> res_;
};