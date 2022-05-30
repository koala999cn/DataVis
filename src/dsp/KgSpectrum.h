#pragma once
#include <memory>
#include "kDsp.h"

/// 频谱分析功能类

class KcSampled1d;
class KvData;

class KgSpectrum
{
public:

	enum KeType
	{
		k_power, // |FFT|^2
		k_log,   // log(|FFT|^2)
		k_db,    // 10*log10(|FFT|^2)
		k_mag    // |FFT|
	};

	KgSpectrum();
	~KgSpectrum();

	// 重置为采样频率为1/dt、数量为count的时域数据执行fft操作
	void reset(kReal dt, kIndex count);

	void porcess(const KvData& samp, KcSampled1d& spec) const;


	// 处理单通道规范化数据
	// samp.size == rdft_.sizeT(), spec.size == rdft_.sizeF()
	void porcess(kReal* data/*inout*/) const;

	int type() const { return type_; }
	void setType(int type) { type_ = type; }

	kReal floor() const { return floor_; }
	void setFloor(kReal f) { floor_ = f; }

	kReal df() const { return df_; }
	kReal nyqiustFreq() const { return nyquistFreq_; }
	unsigned sizeInTime() const;
	unsigned sizeInFreq() const;

private:
	void* rdft_;
	kReal df_;
	kReal nyquistFreq_;
	int type_; // 频谱类型
	kReal floor_; // 频谱底值，type_为k_log或k_db时有效
};
