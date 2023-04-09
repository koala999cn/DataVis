#pragma once
#include "KvTicker.h"
#include "KpContext.h"


class KvNumericTicker : public KvTicker
{
public:

	KvNumericTicker();

	unsigned ticksTotal() const override { return ticks_.size(); }

	unsigned subticksTotal() const override { return subticks_.size(); }

	double tick(unsigned idx) const override { return ticks_[idx]; }

	double subtick(unsigned idx) const override { return subticks_[idx]; }

	std::string label(unsigned idx) const override;

	const KpNumericFormatter& formatter() const { return formatter_; }
	KpNumericFormatter& formatter() { return formatter_; }

protected:

	// Returns the decimal mantissa of @input.
	// Optionally, if @magnitude is not set to zero, it also returns the magnitude of @input as a power of 10.
	// For example, an input of 142.6 will return a mantissa of 1.426 and a magnitude of 100.
	static double getMantissa_(double input, double* magnitude);

	// Returns a number that is close to @input but has a clean, easier human readable mantissa.
	double cleanMantissa_(double input) const;

	static void trimTicks_(double lower, double upper, std::vector<double>& ticks);

	// 用于生成副刻度的帮助函数
	// @subticks: 主刻度之间的副刻度数
	void genSubticks_(unsigned subticks);

protected:

	// 以下为生成的刻度值, 要求子类继承update方法进行填充
	std::vector<double> ticks_;
	std::vector<double> subticks_;

private:
	// A increasing ordered vector of values representing
	// mantisse values between 1 and 9.
	std::vector<double> mantissi_;

	KpNumericFormatter formatter_;
};
