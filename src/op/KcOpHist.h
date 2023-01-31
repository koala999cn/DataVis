#pragma once
#include "KvOp1to1.h"
#include <memory>


class KgHist;

class KcOpHist : public KvOp1to1
{
	using super_ = KvOp1to1;

public:
	KcOpHist();

	int spec(kIndex outPort) const final;

	kRange range(kIndex outPort, kIndex axis) const final;

	kReal step(kIndex outPort, kIndex axis) const final;

	kIndex size(kIndex outPort, kIndex axis) const final;

	bool onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins) final;

	void onStopPipeline() final;

	void showProperySet() final;

	bool permitInput(int dataSpec, unsigned inPort) const final;

	bool onInputChanged(KcPortNode* outPort, unsigned inPort) final;

private:
	void outputImpl_() final;

private:
	std::unique_ptr<KgHist> hist_;
	double min_, max_; // 做hist的值域范围
	int bins_; // hist的bin数
};

