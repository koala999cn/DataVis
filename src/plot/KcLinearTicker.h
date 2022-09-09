#pragma once
#include "KvTicker.h"


class KcLinearTicker : public KvTicker
{
public:
	std::vector<double> apply(double lower, double upper) override;

	void autoRange(double& lower, double& upper) override;
};
