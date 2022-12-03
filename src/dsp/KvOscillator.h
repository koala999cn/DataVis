#pragma once
#include "KvExcitor.h"


// 周期振荡信号发生器的基类

class KvOscillator : public KvExcitor
{
public:

	KvOscillator(kReal period) : period_(period) {}

	void reset(kReal phase) { phase_ = phase; }

	kReal freq() const { return freq_; }

	void setFreq(kReal freq) { freq_ = freq; }

	kReal tickRate() const { return tickRate_; }

	void setTickRate(kReal rate) { tickRate_ = rate; }

	kReal tick() override;

	std::pair<kReal, kReal> range() const override {
		return { -1, 1 };
	}

	kReal period() const { return period_; }

private:

	// 此处的phase已进行归一化，位于[0, period_)区间
	virtual kReal tickImpl_(kReal phase) const = 0;

private:
	kReal period_; // 振荡信号的标准周期
	kReal phase_{ 0 };
	kReal freq_{ 440 }; // 信号自身的频率，用于调节周期
	kReal tickRate_{ 44100 }; // 信号发生的频率，即采样频率
};

