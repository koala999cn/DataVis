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

	void output() final;

	void showProperySet() final;

	bool permitInput(int dataSpec, unsigned inPort) const final;


private:
	int interpMethod_{ 0 };
	int extrapMethod_{ 0 };
};

