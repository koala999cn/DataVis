#pragma once
#include "KvData.h"


// 采样数据的抽象类

class KvSampled : public KvData
{
public:
	virtual void resize(kIndex shape[]) = 0;

	// 调整第axis轴的采样参数
	virtual void reset(kIndex axis, kReal low, kReal step, kReal x0_ref = 0) = 0;
};