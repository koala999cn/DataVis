#pragma once
#include "KvTicker.h"


class KcLinearTicker : public KvTicker
{
public:
	KcLinearTicker();

	void generate(double lower, double upper, bool genSubticks, bool genLabels) override;

private:

	// 根据坐标范围，以及设定的tickCount_，计算易读的主刻度间隔
	double getTickStep_(double lower, double upper) const;

	unsigned autoRange_(double& lower, double& upper);

	static void trimTicks_(double lower, double upper, std::vector<double>& ticks);

private:

	// A increasing ordered vector of values representing
	// mantisse values between 1 and 9.
	std::vector<double> mantissi_;
};
