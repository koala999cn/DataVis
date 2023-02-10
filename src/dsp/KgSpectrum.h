#pragma once
#include <vector>
#include <memory>

class KgRdft;

/// 频谱分析功能类

class KgSpectrum
{
public:

	// 支持的频谱类型
	enum KeType
	{
		k_power, // |FFT|^2, praat模式
		k_log,   // log(|FFT|^2), kaldi模式
		k_db,    // 10*log10(|FFT|^2)
		k_mag    // |FFT|
	};

	// 频谱归一化模式
	enum KeNormMode
	{
		k_norm_none, // 不作归一化，节省计算量
		k_norm_default, // 用时域信号长度N作归一化(KgRdft可控参数)
		k_norm_praat, // 用采样频率F作归一化(兼容praat)
		k_norm_kaldi // 用int16最大值作归一化(兼容kaldi)，非规范操作，慎用
	};

	// @frameSize: 输入数据的长度
	struct KpOptions
	{
		unsigned frameSize;
		double sampleRate;

		KeType type; 
		KeNormMode norm; 
		bool roundToPower2;
	};

	KgSpectrum() = delete;
	KgSpectrum(KgSpectrum&& spec) noexcept;
	KgSpectrum(const KpOptions& opts);
	~KgSpectrum();

	void reset(const KpOptions& opts);

	// 返回输入输出的规格
	unsigned idim() const;
	unsigned odim() const;

	void process(const double* in, double* out) const;

	const KpOptions& options() const { return opts_; }

	// 对功率谱数据data进行归一化和类型转换
	void fixPower(double* spec, unsigned c, bool hasNormDefault = true) const;

	// 给定输入数值范围，计算输出数值范围
	std::pair<double, double> orange(const std::pair<double, double>& in) const;


	/// 帮助函数

	// 计算谱输出维度
	static unsigned odim(unsigned idim, bool roundToPower2);

	static constexpr unsigned typeCount() { return 4; }
	static const char* type2Str(int type);
	static KeType str2Type(const char* str);

	static constexpr unsigned normModeCount() { return 4; }
	static const char* norm2Str(int norm);
	static KeNormMode str2Norm(const char* str);

private:
	KgRdft* rdft_;
	KpOptions opts_;
};
