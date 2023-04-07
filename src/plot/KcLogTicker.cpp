#include "KcLogTicker.h"
#include <cmath>


void KcLogTicker::generate(double lower, double upper, bool genSubticks, bool genLabels)
{
	super_::generate(std::log(lower), std::log(upper), genSubticks, genLabels);
	for (auto& i : ticks_)
		i = std::exp(i);

	if (genSubticks) {
		for (auto& i : subticks_)
			i = std::exp(i);
	}
}


std::string KcLogTicker::genLabel_(double val) const
{
	return super_::genLabel_(std::exp(val));
}
