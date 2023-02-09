#pragma once
#include "KcPvData.h"


/// 多值表达式

class KcPvFunction : public KcPvData
{
	using super_ = KcPvData;

public:
	KcPvFunction();

	void showPropertySet() override;

	// 缓存range计算
	kRange range(kIndex outPort, kIndex axis) const override;

	void notifyChanged(unsigned outPort = -1) override;

private:

	// 更新数据的采样参数
	void updateDataSamplings_();

private:
	std::vector<std::string> exprs_; // 各输出维度的运算表达式
	std::pair<float, float> iranges_[2]; // 采样区间
	int counts_[2]; // 采样点数
	kRange oranges_[2]; // 缓存输出数据dim0，dim1的值域范围. NB: dim2作为valueRange已由KcPvData缓存
};
