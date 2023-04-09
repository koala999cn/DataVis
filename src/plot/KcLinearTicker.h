#pragma once
#include "KvNumericTicker.h"


class KcLinearTicker : public KvNumericTicker
{
	using super_ = KvNumericTicker;

public:
	
	using super_::super_;

	void update(double lower, double upper, bool skipSubticks) override;

private:

	// 根据坐标范围，以及设定的tickCount_，计算易读的主刻度间隔
	double getTickStep_(double lower, double upper) const;

	unsigned autoRange_(double& lower, double& upper);
};
