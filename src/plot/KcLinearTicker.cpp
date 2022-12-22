#include "KcLinearTicker.h"
#include <cmath>
#include <assert.h>
#include "KtuMath.h"


KcLinearTicker::KcLinearTicker()
{
    mantissi_ = //{ 1, 2, 2.5, 5, 10 }; 
     { 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 6.0, 8.0, 10.0 };
}


void KcLinearTicker::generate(double lower, double upper, bool genSubticks, bool genLabels)
{
	if (lower >= upper)
		return;

    auto start = lower;
    auto stop = upper;

	auto tickCount = autoRange_(lower, upper);
	if (tickCount == 0)
		return;

	ticks_.resize(tickCount);
	ticks_.front() = lower;
	auto tickStep = (upper - lower) / (tickCount - 1);
	for (unsigned i = 1; i < tickCount - 1; i++)
		ticks_[i] = lower + i * tickStep;
	ticks_.back() = upper; // ∑¿÷π¿€º∆ŒÛ≤Ó

	if (genSubticks && subtickCount() > 0 && tickCount > 1) {
        subticks_.clear();
        subticks_.reserve(subtickCount() * (tickCount - 1));
		for (unsigned i = 0; i < tickCount - 1; i++) {
            auto subtickStep = tickStep / (subtickCount() + 1);
            for (unsigned j = 1; j <= subtickCount(); j++)
                subticks_.push_back(ticks_[i] + j * subtickStep);
		}
	}

    trimTicks_(start, stop, ticks_);
    trimTicks_(start, stop, subticks_);

    if (genLabels) {
        labels_.resize(ticks_.size());
        for (unsigned i = 0; i < ticks_.size(); i++)
            labels_[i] = genLabel_(ticks_[i]);
    }
}


unsigned KcLinearTicker::autoRange_(double& lower, double& upper)
{
    assert(upper > lower);

    // Generate tick positions according to tickStep:
    auto tickStep = getTickStep_(lower, upper);
    auto firstStep = std::floor(lower / tickStep);
    auto lastStep = std::ceil(upper / tickStep); 
    int tickCount = int(lastStep - firstStep + 1);
    if (tickCount < 0) tickCount = 0;
    lower = firstStep * tickStep;
    upper = lastStep * tickStep;

    return tickCount;
}


double KcLinearTicker::getTickStep_(double lower, double upper) const
{
    if (tickCount() <= 1)
        return upper - lower;

    double exactStep = (upper - lower) / (tickCount() - 1);

    auto exp = KtuMath<double>::floorLog10(exactStep);
    auto mag = std::pow(10, exp);
    auto mantissa = exactStep / mag;

    return mag * KtuMath<double>::pickNearest(mantissa, mantissi_.data(), mantissi_.size());
}


void KcLinearTicker::trimTicks_(double lower, double upper, std::vector<double>& ticks)
{
    // TODO: ”≈ªØ
    while (!ticks.empty() && ticks.front() < lower)
        ticks.erase(ticks.cbegin());

    while (!ticks.empty() && ticks.back() > upper)
        ticks.pop_back();
}
