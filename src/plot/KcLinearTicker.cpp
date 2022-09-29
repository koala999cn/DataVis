#include "KcLinearTicker.h"


std::vector<double> KcLinearTicker::getTicks(double lower, double upper, unsigned ticks)
{
	if (ticks == 0)
		ticks = ticks_;

	std::vector<double> res;
	res.reserve(ticks);

	// first tic
	res.push_back(lower);

	// remaining tics
	if (ticks > 1) {

		auto dx = (upper - lower) / (ticks - 1);
		for (unsigned i = 1; i < ticks - 1; ++i)
		{
			auto x = lower + i * dx;
			res.push_back(x);
		}
		res.push_back(upper);

	}

	return res;
}


void KcLinearTicker::autoRange(double& lower, double& upper)
{

}

