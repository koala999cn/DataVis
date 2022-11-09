#pragma once
#include "KvDataOperator.h"
#include <vector>

class KcSampler;

// 采样器实现：将连续数据和数组数据转换为采样数据

class KcOpSampler : public KvDataOperator
{
	using super_ = KvDataOperator;

public:
	KcOpSampler();

	int spec(kIndex outPort) const final;

	bool onNewLink(KcPortNode* from, KcPortNode* to) final;

	bool onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins) final;

	void onStopPipeline() final;

	void output() final;

	void showProperySet() final;

	bool permitInput(int dataSpec, unsigned inPort) const final;

	bool onInputChanged(KcPortNode* outPort, unsigned inPort) final;


private:
	std::shared_ptr<KcSampler> sampler_;
};

