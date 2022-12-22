#pragma once
#include "KvTicker.h"


class KcLinearTicker : public KvTicker
{
public:
	KcLinearTicker();

	void generate(double lower, double upper, bool genSubticks, bool genLabels) override;

private:

	// �������귶Χ���Լ��趨��tickCount_�������׶������̶ȼ��
	double getTickStep_(double lower, double upper) const;

	unsigned autoRange_(double& lower, double& upper);

	static void trimTicks_(double lower, double upper, std::vector<double>& ticks);

private:

	// A increasing ordered vector of values representing
	// mantisse values between 1 and 9.
	std::vector<double> mantissi_;
};
