#pragma once
#include <memory>
#include "KvContinued.h"

class KvDiscreted;

class KcInterpolater : public KvContinued
{
public:
	KcInterpolater(std::shared_ptr<KvDiscreted>& cont);

	enum KeInterMethod
	{
		k_linear,
		k_quad
	};

	int interMethod() const { return interMethod_; }
	void setInterMethod(int m) {
		interMethod_ = m;
	}

	enum KeExtraMethod
	{
		k_nan,    // 返回nan值
		k_zero,   // 返回0值
		k_const,  // 返回边界值
		k_mirro,  // 返回镜像值
		k_period, // 返回周期值
		k_extra   // 返回外推值（使用内插方法）
	};

	int extraMethod() const { return extraMethod_; }
	void setExtraMethod(int m) {
		extraMethod_ = m;
	}


	kIndex dim() const override;

	kIndex channels() const override;

	kRange range(kIndex axis) const override;

	kReal value(kReal pt[], kIndex channel) const override;

private:
	const std::shared_ptr<KvDiscreted> internal_;

	int interMethod_; // 内插方法
	int extraMethod_; // 外插方法
};

