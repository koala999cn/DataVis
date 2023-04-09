#include "KcLinearTicker.h"
#include <assert.h>
#include "KuMath.h"


void KcLinearTicker::update(double lower, double upper, bool skipSubticks)
{
	if (lower >= upper)
		return;

    auto start = lower;
    auto stop = upper;

	auto tickCount = autoRange_(lower, upper);
	ticks_.resize(tickCount);
    if (ticks_.empty()) {
        subticks_.clear();
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

    if (!skipSubticks && subticksExpected() > 0)
        genSubticks_(subticksExpected());

    trimTicks_(start, stop, ticks_);
    trimTicks_(start, stop, subticks_);
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
    if (ticksExpected() <= 1)
        return upper - lower;

    double exactStep = (upper - lower) / (ticksExpected() - 1);
    return cleanMantissa_(exactStep);
}

