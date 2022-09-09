#include "KcLinearTicker.h"


std::vector<double> KcLinearTicker::apply(double lower, double upper)
{
	std::vector<double> res;
	res.reserve(ticks());

	// first tic
	res.push_back(lower);

	// remaining tics
	if (ticks() > 1) {

		auto dx = (upper - lower) / (ticks() - 1);
		for (unsigned i = 1; i < ticks() - 1; ++i)
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

