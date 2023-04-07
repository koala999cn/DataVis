#pragma once
#include "KcLinearTicker.h"


class KcLogTicker : public KcLinearTicker
{
	using super_ = KcLinearTicker;

public:
	KcLogTicker();

	void generate(double lower, double upper, bool genSubticks, bool genLabels) override;

	// Sets the logarithm base used for tick coordinate generation. 
	// The ticks will be placed at integer powers of @base.
	void setLogBase(double base);

private:
	double base_;
	double logToBase_; // �������������ִ��log[e]��log[base]��ת��
};
