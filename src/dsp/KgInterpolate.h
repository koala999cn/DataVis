#pragma once
#include "kDsp.h"

class KvData;

class KgInterpolate
{
public:
	KgInterpolate();

	enum KeInterMethod
	{
		k_linear,
		k_quad,
		k_hermite,
		k_cubic,
		k_sinc7,
		k_sinc70,
		k_sinc700
	};

	enum KeExtraMethod
	{
		k_nan,    // 返回nan值
		k_zero,   // 返回0值
		k_const,  // 返回边界值
		k_mirro,  // 返回镜像值
		k_period, // 返回周期值
		k_extra   // 返回外推值（使用内插方法）
	};

	kReal process(const KvData& data, kReal x, kIndex channel);

private:
	int interMethod_; // 内插方法
	int extraMethod_; // 外插方法
};

