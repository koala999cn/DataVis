#pragma once
#include "KvTicker.h"


class KcLinearTicker : public KvTicker
{
public:
	std::vector<double> getTicks(double lower, double upper, unsigned ticks = 0) override;

	void autoRange(double& lower, double& upper) override;
};
