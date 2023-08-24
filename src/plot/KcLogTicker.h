#pragma once
#include "KcLinearTicker.h"

// 实现log域数值刻度

class KcLogTicker : public KcLinearTicker
{
	using super_ = KcLinearTicker;

public:
	KcLogTicker();

	void update(double lower, double upper, bool skipSubticks) override;

	double map(double val) const override;

	// Sets the logarithm base used for tick coordinate generation. 
	// The ticks will be placed at integer powers of @base.
	void setLogBase(double base);

private:
	double base_;
	double logToBase_; // 缓存变量，用来执行log[e]到log[base]的转换
};
