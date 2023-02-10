#pragma once
#include "KvDataOperator.h"
#include <memory>


class KgHist;

class KcOpHist : public KvDataOperator
{
	using super_ = KvDataOperator;

public:
	KcOpHist();

	int spec(kIndex outPort) const final;

	kRange range(kIndex outPort, kIndex axis) const final;

	kReal step(kIndex outPort, kIndex axis) const final;

	kIndex size(kIndex outPort, kIndex axis) const final;

	bool onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins) final;

	void onStopPipeline() final;

	void showPropertySet() final;

	bool permitInput(int dataSpec, unsigned inPort) const final;

	bool onInputChanged(KcPortNode* outPort, unsigned inPort) final;

private:

	void prepareOutput_() final;

	void outputImpl_() final;

private:
	std::unique_ptr<KgHist> hist_;
	double min_, max_; // 做hist的值域范围
	int bins_; // hist的bin数
};

