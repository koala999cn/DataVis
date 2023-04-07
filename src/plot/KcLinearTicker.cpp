#include "KcLinearTicker.h"
#include <cmath>
#include <assert.h>
#include "KuMath.h"


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
	ticks_.resize(tickCount);
    if (ticks_.empty()) {
        subticks_.clear();
        labels_.clear();
        return;
    }

	ticks_.front() = lower;
	auto tickStep = (upper - lower) / (tickCount - 1);
    for (unsigned i = 1; i < tickCount - 1; i++) {
        ticks_[i] = lower + i * tickStep;

        // NB: 处理因累计误差而产生的极小值（打印label时会产生长串字符，诸如2.12342e-16）
        if (ticks_[i] < tickStep / 1000 && KuMath::almostEqual(ticks_[i], 0.))
            ticks_[i] = 0;
    }
	ticks_.back() = upper; // 防止累计误差

    if (genSubticks && subtickCount() > 0)
        genSubticks_();

    trimTicks_(start, stop, ticks_);
    trimTicks_(start, stop, subticks_);

    if (genLabels)
        genLabels_();
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


double KcLinearTicker::getMantissa_(double input, double* magnitude)
{
    const double mag = std::pow(10.0, KuMath::floorLog10(input));
    if (magnitude) *magnitude = mag;
    return input / mag;
}


double KcLinearTicker::cleanMantissa_(double input) const
{
    double magnitude;
    auto mantissa = getMantissa_(input, &magnitude);
    return magnitude * KuMath::pickNearest(mantissa, mantissi_.data(), mantissi_.size());
}


double KcLinearTicker::getTickStep_(double lower, double upper) const
{
    if (tickCount() <= 1)
        return upper - lower;

    double exactStep = (upper - lower) / (tickCount() - 1);
    return cleanMantissa_(exactStep);
}


void KcLinearTicker::trimTicks_(double lower, double upper, std::vector<double>& ticks)
{
    // TODO: 优化
    while (!ticks.empty() && ticks.front() < lower)
        ticks.erase(ticks.cbegin());

    while (!ticks.empty() && ticks.back() > upper)
        ticks.pop_back();
}


void KcLinearTicker::genSubticks_()
{
    subticks_.clear();
    subticks_.reserve(subtickCount() * (ticks_.size() - 1));
    for (unsigned i = 0; i < ticks_.size() - 1; i++) {
        auto subtickStep = (ticks_[i + 1] - ticks_[i]) / (subtickCount() + 1);
        for (unsigned j = 1; j <= subtickCount(); j++)
            subticks_.push_back(ticks_[i] + j * subtickStep);
    }
}


void KcLinearTicker::genLabels_()
{
    labels_.resize(ticks_.size());
    for (unsigned i = 0; i < ticks_.size(); i++)
        labels_[i] = genLabel_(ticks_[i]);
}
