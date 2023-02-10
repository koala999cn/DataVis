#pragma once
#include "KvDataOperator.h"


class KcOpInterpolater : public KvDataOperator // 输出连续数据，不继承KvOpSampled1dHelper
{
	using super_ = KvDataOperator;

public:
	KcOpInterpolater();

	int spec(kIndex outPort) const final;

	kReal step(kIndex outPort, kIndex axis) const final;

	kIndex size(kIndex outPort, kIndex axis) const final;

	void showPropertySet() final;

	bool permitInput(int dataSpec, unsigned inPort) const final;

private:

	void prepareOutput_() final {}

	void outputImpl_() final;

private:
	int interpMethod_{ 0 };
	int extrapMethod_{ 0 };
};

