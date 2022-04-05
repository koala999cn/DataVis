#pragma once
#include <vector>
#include "kDsp.h"

class KcSampled1d;


// 滤波器组分析：只适用于均匀采样数据
// 滤波器的各子带(bin)在目标频率尺度上始终是均匀划分的
 
class KgFilterBank
{
public:

	// FBank类型
	enum KeType
	{
		k_linear,
		k_log,
		k_mel,
		k_bark,
		k_erb
	};


	KgFilterBank(int type = k_mel);
	
	// @df: 输入频谱的df
	// @low, @high: 拟作fbank分析的频率范围(Hz)
	void reset(int type, kIndex numBins, kReal df, kReal low, kReal high);

	// @out: 尺寸等于numBins
	void process(const KcSampled1d& in, kReal* out); 

	// @in: 输入频谱，从0Hz开始，间隔df_，共N个频点
	void process(const kReal* in, unsigned N, kReal* out);

	int type() const { return type_; }
	auto numBins() const { return fc_.size(); }

	// 返回第bin个子带的中心频率(Hz)
	kReal fc(kIndex bin) const { return fc_[bin]; }

private:
	kReal toHertz_(kReal scale); // 将type频率尺度转换为hz
	kReal fromHertz_(kReal hz); // 将hz转换为type频率尺度
	static kReal calcFilterWeight_(kReal low, kReal high, kReal f);

private:
	int type_;
	std::vector<kIndex> firstIdx_; // 各bin的起始点位置
	std::vector<kReal> fc_; // 各bin的中心点频率值(Hz)
	std::vector<std::vector<kReal>> weights_; // 各bin的权值数组，数组大小等于各bin的采样点数
};
