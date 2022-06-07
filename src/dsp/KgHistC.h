#pragma once
#include "KmBins.h"


// 直方图计数

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