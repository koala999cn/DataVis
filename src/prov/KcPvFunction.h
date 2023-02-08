#pragma once
#include "KcPvData.h"


/// 多值表达式

class KcPvFunction : public KcPvData
{
	using super_ = KcPvData;

public:
	KcPvFunction();

	void showPropertySet() override;

private:

	// 更新数据的采样参数
	void updateDataSamplings_();

private:
	std::vector<std::string> exprs_; // 各输出维度的运算表达式
	std::pair<float, float> ranges_[2];
	int counts_[2];
};
