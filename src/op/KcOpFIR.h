#pragma once
#include "KvOp1to1.h"
#include <memory>
#include "dsp/KtFIR.h"


class KcOpFIR : public KvOp1to1
{
	using super_ = KvOp1to1;

public:
	KcOpFIR();

	bool onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins) final;

	void onStopPipeline() final;

	void showProperySet() final;

	bool permitInput(int dataSpec, unsigned inPort) const final;


private:
	void outputImpl_() final;

	void createFilter_();

private:
	std::unique_ptr<KtFIR<kReal>> filter_;
	int type_; // 滤波器类型：低通、高通、带通、带阻等
	int window_; // 窗类型
	int taps_;
	float cutoff0_, cutoff1_;  // 两个截至频率，取值0 - 0.5
	bool dirty_; // filter的配置参数是否发生变化
};
