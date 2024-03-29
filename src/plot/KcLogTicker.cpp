#include "KcLogTicker.h"
#include "KuMath.h"


KcLogTicker::KcLogTicker()
{
	setLogBase(10.);
}


// �ο�QCustomPlotʵ��
void KcLogTicker::update(double lower, double upper, bool skipSubticks)
{
    assert(upper > lower);

    if (KuMath::sign(lower) != KuMath::sign(upper))
        return;

    ticks_.clear();

    if (lower > 0 && upper > 0) { // positive range
        const double baseTickCount = std::log(upper / lower) * logToBase_;
        if (baseTickCount < 1.6) // if too few log ticks would be visible in axis range, fall back to regular tick vector generation
            return super_::update(lower, upper, skipSubticks);

        const double exactPowerStep = baseTickCount / double(ticksExpected() + 1e-10);
        const double newLogBase = std::pow(base_, std::max(int(cleanMantissa_(exactPowerStep)), 1));
        double currentTick = std::pow(newLogBase, std::floor(std::log(lower) / std::log(newLogBase)));
        ticks_.push_back(currentTick);
        while (currentTick < upper && currentTick > 0) { // currentMag might be zero for ranges ~1e-300, just cancel in that case
            currentTick *= newLogBase;
            ticks_.push_back(currentTick);
        }
    }
    else { // negative range
        const double baseTickCount = std::log(lower / upper) * logToBase_;
        if (baseTickCount < 1.6) // if too few log ticks would be visible in axis range, fall back to regular tick vector generation
            return super_::update(lower, upper, skipSubticks);

        const double exactPowerStep = baseTickCount / double(ticksExpected() + 1e-10);
        const double newLogBase = std::pow(base_, std::max(int(cleanMantissa_(exactPowerStep)), 1));
        double currentTick = -std::pow(newLogBase, std::ceil(std::log(-lower) / std::log(newLogBase)));
        ticks_.push_back(currentTick);
        while (currentTick < upper && currentTick < 0) { // currentMag might be zero for ranges ~1e-300, just cancel in that case
            currentTick /= newLogBase;
            ticks_.push_back(currentTick);
        }
    }

    if (!skipSubticks && subticksExpected() > 0)
        genSubticks_(subticksExpected());

    trimTicks_(lower, upper, ticks_);
    trimTicks_(lower, upper, subticks_);
}


double KcLogTicker::map(double val) const
{
    return std::log(val) * logToBase_;
}


void KcLogTicker::setLogBase(double base)
{
	base_ = base;
	logToBase_ = KuMath::logToBase(1., base);
}
