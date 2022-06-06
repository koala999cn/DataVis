#pragma once
#include "KmBins.h"

class KcSampled1d;
class KvData;


// 均值直方图：每个bin的数值为累积数据的平均值

class KgHist : public KmBins
{
public:
	KgHist() : KmBins() {}
	

	// 适用于in均匀间隔采样，bin线性尺度
	void process(const KcSampled1d& in, KcSampled1d& out); 

	// 适用于in均匀间隔采样，bin不要求线性尺度
	// @out: 大小等于numBins()*in.channels()
	void process(const KcSampled1d& in, kReal* out);

	// 假定in[0]的x值为首个bin的左边界，in[len-1]的x值为末尾bin的右边界
	void process(const kReal* in, unsigned len, kReal* out);

	// 更一般的情况，既不要求in均匀间隔采样，也不要求bin线性尺度
	void process(const KvData& in, kReal* out);
};