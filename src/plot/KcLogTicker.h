#pragma once
#include "KcLinearTicker.h"


class KcLogTicker : public KcLinearTicker
{
	using super_ = KcLinearTicker;

public:
	using super_::super_;

	void generate(double lower, double upper, bool genSubticks, bool genLabels) override;

private:
	std::string genLabel_(double val) const override;
};
