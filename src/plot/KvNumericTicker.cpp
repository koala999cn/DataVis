#include "KvNumericTicker.h"
#include "KuMath.h"
#include <sstream>


KvNumericTicker::KvNumericTicker()
{
	mantissi_ = //{ 1, 2, 2.5, 5, 10 }; 
		{ 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 6.0, 8.0, 10.0 };
}


double KvNumericTicker::getMantissa_(double input, double* magnitude)
{
	const double mag = std::pow(10.0, KuMath::floorLog10(input));
	if (magnitude) *magnitude = mag;
	return input / mag;
}


double KvNumericTicker::cleanMantissa_(double input) const
{
	double magnitude;
	auto mantissa = getMantissa_(input, &magnitude);
	return magnitude * KuMath::pickNearest(mantissa, mantissi_.data(), mantissi_.size());
}


void KvNumericTicker::trimTicks_(double lower, double upper, std::vector<double>& ticks)
{
	// TODO: 优化
	while (!ticks.empty() && ticks.front() < lower)
		ticks.erase(ticks.cbegin());

	while (!ticks.empty() && ticks.back() > upper)
		ticks.pop_back();
}


void KvNumericTicker::genSubticks_(unsigned subticks)
{
	subticks_.clear();
	subticks_.reserve(subticks * (ticksTotal() - 1));
	for (unsigned i = 0; i < ticksTotal() - 1; i++) {
		auto subtickStep = (tick(i + 1) - tick(i)) / (subticks + 1);
		for (unsigned j = 1; j <= subticks; j++)
			subticks_.push_back(tick(i) + j * subtickStep);
	}
}


std::string KvNumericTicker::label(unsigned idx) const
{
	std::ostringstream strm;
	formatter_.apply(strm);
	strm << tick(idx);

	// 将"0.00000..."替换为"0"
	//std::regex re("^[+-]?0[\\.,]?0*$");
	//if (std::regex_match(buf, re))
	//	buf[0] = '0', buf[1] = '\0';

	return strm.str();
}

