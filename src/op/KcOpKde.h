#pragma once
#include "KvDataOperator.h"


class KcOpKde : public KvDataOperator // 输出连续数据，不继承KvOpSampled1dHelper
{
	using super_ = KvDataOperator;

public:
	KcOpKde();

	int spec(kIndex outPort) const final;

	kReal step(kIndex outPort, kIndex axis) const final;

	kIndex size(kIndex outPort, kIndex axis) const final;

	kRange range(kIndex outPort, kIndex axis) const final;

	void showPropertySet() final;

	bool permitInput(int dataSpec, unsigned inPort) const final;

private:

	bool prepareOutput_() final { return false; }

	void outputImpl_() final;
};