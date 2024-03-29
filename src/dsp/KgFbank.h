﻿#pragma once
#include <vector>


// 滤波器组分析：只适用于均匀采样数据
// 滤波器的各子带(bin)在目标频率尺度上始终是均匀划分的
 
class KgFbank
{
public:

	// Fbank类型
	enum KeType
	{
		k_linear,
		k_log,
		k_mel,
		k_bark,
		k_erb,
		k_type_count
	};

	// @lowFreq, @highFreq: 拟作fbank分析的频率范围(Hz)
	struct KpOptions
	{
		double sampleRate;
		unsigned fftBins; // 输入的采样点数，即idim

		KeType type; 
		unsigned numBanks;
		double lowFreq;
		double highFreq;
		bool normalize; // 若true，则每个bank的滤波结果将除以bank宽度(Hertz域)
	};

	KgFbank() = default;
	KgFbank(KgFbank&& fbank) noexcept;
	KgFbank(const KpOptions& opts);

	// 返回输入、输出的dim
	unsigned idim() const;
	unsigned odim() const;

	// @in: 输入频谱数据[0, nf], 共idim_个频点
	void process(const double* in, double* out) const;

	// 返回第idx个子带的中心频率(Hz)
	double fc(unsigned idx) const { return fc_[idx]; }

	const KpOptions& options() const { return opts_; }

	void setNormalize(bool b);

	// 目标尺度
	auto rangeInScale() const { return range_; }

	// 目标尺度
	double stepInScale() const { return binStep_; }


	/// 帮助函数

	static const char* type2Str(KeType type);
	static KeType str2Type(const char* str);

	static double toHertz(KeType type, double scale); // 将type频率尺度转换为hz
	static double fromHertz(KeType type, double hz); // 将hz转换为type频率尺度

private:

	void initWeights_();

	static double calcFilterWeight_(double low, double high, double f);

private:
	KpOptions opts_;
	std::vector<unsigned> firstIdx_; // 各bank的起始点位置
	std::vector<double> fc_; // 各bank的中心点频率值(Hz)
	std::vector<std::vector<double>> weights_; // 各bank的权值数组，数组大小等于各bank的采样点数

	// 根据opts计算的缓存变量
	std::pair<double, double> range_;
	double binStep_;
};
