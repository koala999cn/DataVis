#pragma once
#include "KvTicker.h"


class KcLinearTicker : public KvTicker
{
public:
	KcLinearTicker();

	void generate(double lower, double upper, bool genSubticks, bool genLabels) override;

protected:

	// Returns the decimal mantissa of @input.
	// Optionally, if @magnitude is not set to zero, it also returns the magnitude of @input as a power of 10.
	// For example, an input of 142.6 will return a mantissa of 1.426 and a magnitude of 100.
	static double getMantissa_(double input, double* magnitude);

	// Returns a number that is close to @input but has a clean, easier human readable mantissa.
	double cleanMantissa_(double input) const;

	void genSubticks_();

	void genLabels_();

	static void trimTicks_(double lower, double upper, std::vector<double>& ticks);

private:

	// 根据坐标范围，以及设定的tickCount_，计算易读的主刻度间隔
	double getTickStep_(double lower, double upper) const;

	unsigned autoRange_(double& lower, double& upper);

private:

	// A increasing ordered vector of values representing
	// mantisse values between 1 and 9.
	std::vector<double> mantissi_;
};
