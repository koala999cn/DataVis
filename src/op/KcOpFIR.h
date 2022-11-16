#pragma once
#include "KvDataOperator.h"
#include <memory>
#include "dsp/KtFIR.h"


class KcOpFIR : public KvDataOperator
{
	using super_ = KvDataOperator;

public:
	KcOpFIR();

	bool onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins) final;

	void onStopPipeline() final;

	void output() final;

	void showProperySet() final;

	bool permitInput(int dataSpec, unsigned inPort) const final;


private:
	void createFilter_();

private:
	std::unique_ptr<KtFIR<kReal>> filter_;
	int type_; // 滤波器类型：低通、高通、带通、带阻等
	int window_; // 窗类型
	int taps_;
	float cutoff0_, cutoff1_;  // 两个截至频率，取值0 - 0.5
	bool dirty_; // filter的配置参数是否发生变化
};
