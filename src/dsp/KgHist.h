#pragma once
#include "KmBins.h"

class KvSampled;
class KvData;


// 均值直方图：每个bin的数值为累积数据的平均值
// 
// TODO: 1. 实现2d数据的直方图统计; 2. 测试process(const KvData& in, kReal* out).
// 

class KgHist : public KmBins
{
public:
	KgHist() : KmBins() {}
	
	// 适用于in均匀间隔采样，bin不要求线性尺度
	// @out: 大小等于numBins()*in.channels()
	void process(const KvSampled& in, kReal* out);


	// 更一般的情况，既不要求in均匀间隔采样，也不要求bin线性尺度
	// 可用于统计散点数据
	void process(const KvData& in, kReal* out);
};
