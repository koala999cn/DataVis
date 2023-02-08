#pragma once
#include "KvOp1to1.h"
#include <vector>

class KcSampler;

// 采样器实现：将连续数据和数组数据转换为采样数据

class KcOpSampler : public KvOp1to1
{
	using super_ = KvOp1to1;

public:
	KcOpSampler();

	int spec(kIndex outPort) const final;

	kRange range(kIndex outPort, kIndex axis) const final;

	kReal step(kIndex outPort, kIndex axis) const final;

	kIndex size(kIndex outPort, kIndex axis) const final;

	bool onNewLink(KcPortNode* from, KcPortNode* to) final;

	void showPropertySet() final;

	bool permitInput(int dataSpec, unsigned inPort) const final;

	bool onInputChanged(KcPortNode* outPort, unsigned inPort) final;


private:
	void outputImpl_() final;

	void sampleCountChanged_();

private:
	std::shared_ptr<KcSampler> sampler_;
	std::vector<int> sampCount_; // 适用于连续数据
};

