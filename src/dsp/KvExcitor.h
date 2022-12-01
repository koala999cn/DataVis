#pragma once
#include "kDsp.h"
#include <vector> // for std::pair


// 激励源(Excitation Source)基类
// 激励源逐个产生时序数据

class KvExcitor
{
public:

	// 产生一个激励数据
	virtual kReal tick() = 0;

	// 激励数据的数值范围
	virtual std::pair<kReal, kReal> range() const = 0;
};

