#pragma once

class KvExcitor;

// 随机过程的工厂类

class KuStocahsticFactory
{
public:

	static unsigned typeCount();

	static const char* typeName(unsigned typeIdx);

	static KvExcitor* create(const char* name);

	static KvExcitor* create(unsigned typeIdx);

private:
	KuStocahsticFactory() = delete;
};