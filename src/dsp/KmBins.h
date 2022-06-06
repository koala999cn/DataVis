#pragma once
#include <vector>
#include "kDsp.h"


class KmBins
{
public:
	KmBins() : bins_(1, 0) {}

	// 在[low, high)区间，按线性尺度划分numBins个bin进行数据统计，每个bin的宽度相同
	void resetLinear(kIndex numBins, kReal low, kReal high);

	// 在[low, high)区间，按log尺度划分numBins个bin进行数据统计，每个bin的宽度在log域相同
	void resetLog(kIndex numBins, kReal low, kReal high);

	// 直接设置bins
	void resetCustom(const std::vector<kReal>& bins) {
		bins_ = bins;
	}

	auto range() const -> std::pair<kReal, kReal> {
		return { bins_[0], bins_.back() };
	}

	auto numBins() const {
		return bins_.size() - 1;
	}

	kReal binLeft(kIndex idx) const { return bins_[idx]; }
	kReal binRight(kIndex idx) const { return bins_[++idx]; }
	kReal binCenter(kIndex idx) const {
		return (binLeft(idx) + binRight(idx)) / 2;
	}

	kReal binWidth(kIndex idx) const {
		return binRight(idx) - binLeft(idx);
	}

private:
	std::vector<kReal> bins_; // 按顺序排列的bin边界
};
