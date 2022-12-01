#pragma once
#include "KvExcitor.h"
#include "rchoscillators/rch_oscillators.h"


// 常用振荡器

class KuOscillatorFactory
{
public:

	// 返回支持的振荡器数量
	static unsigned typeCount();

	// 返回第idx个振荡器的名字
	static const char* typeName(unsigned typeIdx);

	// 通过振荡器名字创建
	static KvExcitor* create(const char* name);

	// 通过振荡器类型创建
	static KvExcitor* create(unsigned typeIdx);

private:
	KuOscillatorFactory() = delete;
};
