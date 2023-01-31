#pragma once
#include "KvOp1to1.h"


class KcOpInterpolater : public KvOp1to1
{
	using super_ = KvOp1to1;

public:
	KcOpInterpolater();

	int spec(kIndex outPort) const final;

	kReal step(kIndex outPort, kIndex axis) const final;

	kIndex size(kIndex outPort, kIndex axis) const final;

	void showProperySet() final;

	bool permitInput(int dataSpec, unsigned inPort) const final;

private:
	void outputImpl_() final;

private:
	int interpMethod_{ 0 };
	int extrapMethod_{ 0 };
};

