#pragma once
#include "KvNumericTicker.h"


class KcLinearTicker : public KvNumericTicker
{
	using super_ = KvNumericTicker;

public:
	
	using super_::super_;

	void update(double lower, double upper, bool skipSubticks) override;

private:

	// �������귶Χ���Լ��趨��tickCount_�������׶������̶ȼ��
	double getTickStep_(double lower, double upper) const;

	unsigned autoRange_(double& lower, double& upper);
};
