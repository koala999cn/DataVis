#pragma once
#include <vector>
#include <functional>


class KgPreprocess
{
public:

	enum KeEnergyMode
	{
		k_use_energy_none,
		k_use_energy_raw, // dither & submean之后，preemphasis & windowing之前的能量
		k_use_energy_win // windowing之后的能量
	};

	struct KpOptions
	{
		double sampleRate; // 输入数据的采样频率

		unsigned frameSize; // samples per frame (default = 0.025 * f)
		unsigned frameShift; // Frame shift in samples (default = 0.01 * f)
		
		double dither; // Dithering constant (0.0 means no dither). 
		
		bool removeDcOffset; // Subtract mean from waveform on each frame (default = true)

		double preemphasis; // 0.0 means no preemphasis, default = 0.97
		
		int windowType;
		double windowArg;

		KeEnergyMode useEnergy; // k_use_energy_none表示不计算信号的能量，
		                        // k_use_energy_raw表示在preemphasis和windowing前计算信号能量，
								// k_use_energy_win表示加窗后计算信号能量. 
		                        // E = sum(x[i]*x[i])

		// TODO: kaldi的snip-edges暂不支持，始终默认为true
	};


	KgPreprocess(const KpOptions& opts);

	~KgPreprocess();

	// 设置回调函数，process和flush将逐帧调用该函数
	// h的第一个参数为帧数据指针，第二个参数为帧能量（power energy）
	void setHandler(std::function<void(double*, double)> h);

	void process(const double* buf, unsigned len) const;

	void flush() const;

	// 只有输出维度，输入维度由用户提供
	unsigned odim() const {
		return opts_.frameSize; 
	}

	const KpOptions& options() const { return opts_; }

private:

	double processOneFrame_(const double* in, double* out) const;

private:
	KpOptions opts_;
	void* dptr_;
	std::function<void(double*, double)> handler_;
};

