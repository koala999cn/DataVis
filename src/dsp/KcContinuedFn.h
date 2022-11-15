#pragma once
#include "KvContinued.h"
#include <functional>
#include <vector>
#include <assert.h>

// 基于连续函数的数据类

class KcContinuedFn : public KvContinued
{
public:
	using fn_type = std::function<kReal(kReal[])>;

	KcContinuedFn(std::vector<fn_type> fun, unsigned dim);

	// 单通道
	KcContinuedFn(fn_type fun, unsigned dim);

	// 一维函数
	KcContinuedFn(std::function<kReal(kReal)> fun);

	// 二维函数
	KcContinuedFn(std::function<kReal(kReal, kReal)> fun);

	kIndex dim() const override {
		return static_cast<kIndex>(range_.size() - 1);
	}

	kIndex channels() const override { 
		return static_cast<kIndex>(fun_.size());
	}

	kRange range(kIndex axis) const override;

	kReal value(kReal pt[], kIndex channel) const override;

	void setRange(kIndex axis, kReal low, kReal high) override;

private:
	std::vector<fn_type> fun_; // size等于通道数
	mutable std::vector<kRange> range_; // 大小等于dim+1, range_[dim]表示valueRange.

	enum KeValueRangeStatus
	{
		k_unknown, // 未设置，初始状态
		k_specified, // 已指定，由用户调用setRange设置
		k_esimated, // 估算值，由内部调用基类的valueRange估计
		k_expired // 过期（用户重新设定了区域范围），需要重新估算
	};

	mutable int valueRangeStatus_{ k_unknown };
};
