#pragma once
#include "KvPvExcitor.h"


class KcPvOscillator : public KvPvExcitor
{
	using super_ = KvPvExcitor;

public:

	KcPvOscillator();

	int spec(kIndex outPort) const override;

	kReal step(kIndex outPort, kIndex axis) const override;

	void showProperySet() override;

private:

	unsigned typeCount_() const final;
	const char* typeStr_(int type) const final;
	KvExcitor* createExcitor_(int type) final;

private:

	float tickRate_{ 8000 }; // 产生信号的采样频率
	float freq_{ 100 }; // 产生信号自身的频率
	float phase_{ 0 }; // 初始相位
};
