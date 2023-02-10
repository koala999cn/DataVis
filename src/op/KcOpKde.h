#pragma once
#include "KvOp1to1.h"


class KcOpKde : public KvOp1to1
{
	using super_ = KvOp1to1;

public:
	KcOpKde();

	int spec(kIndex outPort) const final;

	kReal step(kIndex outPort, kIndex axis) const final;

	kIndex size(kIndex outPort, kIndex axis) const final;

	kRange range(kIndex outPort, kIndex axis) const final;

	void showPropertySet() final;

	bool permitInput(int dataSpec, unsigned inPort) const final;

private:
	void outputImpl_() final;
};