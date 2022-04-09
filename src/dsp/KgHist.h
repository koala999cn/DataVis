#pragma once
#include <vector>
#include "kDsp.h"

class KcSampled1d;
class KvData;


// 均值直方图：每个bin的数值为累积数据的平均值

class KgHist
{
public:
	KgHist() : bins_(1, 0) {}
	
	// 在[low, high)区间，划分numBins个bin进行数据统计
	void reset(kIndex numBins, kReal low, kReal high); // 线性尺度，每个bin的宽度相同

	auto range() const -> std::pair<kReal, kReal> {
		return { bins_[0], bins_.back() };
	}

	auto numBins() const {
		return bins_.size() - 1;
	}

	kReal binLeft(kIndex idx) const { return bins_[idx]; }
	kReal binRight(kIndex idx) const { return bins_[idx + 1]; }
	kReal binCenter(kIndex idx) const {
		return (binLeft(idx) + binRight(idx)) / 2;
	}

	kReal binWidth(kIndex idx) const {
		return binRight(idx) - binLeft(idx);
	}


	// 适用于in均匀间隔采样，bin线性尺度
	void process(const KcSampled1d& in, KcSampled1d& out); 

	// 适用于in均匀间隔采样，bin不要求线性尺度
	// @out: 大小等于numBins()*in.channels()
	void process(const KcSampled1d& in, kReal* out);

	// 假定in[0]的x值为首个bin的左边界，in[len-1]的x值为末尾bin的右边界
	void process(const kReal* in, unsigned len, kReal* out);


	// 更一般的情况，既不要求in均匀间隔采样，也不要求bin线性尺度
	void process(const KvData& in, kReal* out);

private:
	std::vector<kReal> bins_; // 按顺序排列的bin边界
};