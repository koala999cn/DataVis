#pragma once
#include "KvDiscreted.h"


// 采样数据的抽象类

class KvSampled : public KvDiscreted
{
public:
	virtual void resize(kIndex shape[]) = 0;

	// 调整第axis轴的采样参数
	virtual void reset(kIndex axis, kReal low, kReal step, kReal x0_ref = 0) = 0;
};