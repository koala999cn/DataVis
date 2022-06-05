#pragma once
#include "kDsp.h"


// 激励源(Excitation Source)基类
// 激励源逐个产生时序数据

class KvExcitor
{
public:
	virtual kReal pulse() = 0;
};

