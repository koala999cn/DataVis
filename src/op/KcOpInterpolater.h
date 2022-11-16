#pragma once
#include "KvDataOperator.h"


class KcOpInterpolater : public KvDataOperator
{
	using super_ = KvDataOperator;

public:
	KcOpInterpolater();

	int spec(kIndex outPort) const final;

	kReal step(kIndex outPort, kIndex axis) const final;

	kIndex size(kIndex outPort, kIndex axis) const final;

	//bool onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins) final;

	//void onStopPipeline() final;

	void output() final;

	void showProperySet() final;

	bool permitInput(int dataSpec, unsigned inPort) const final;

	//bool onInputChanged(KcPortNode* outPort, unsigned inPort) final;


private:
	int interpMethod_{ 0 };
	int extrapMethod_{ 0 };
};

