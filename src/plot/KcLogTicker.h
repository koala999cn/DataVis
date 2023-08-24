#pragma once
#include "KcLinearTicker.h"

// ʵ��log����ֵ�̶�

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
	double logToBase_; // �������������ִ��log[e]��log[base]��ת��
};
