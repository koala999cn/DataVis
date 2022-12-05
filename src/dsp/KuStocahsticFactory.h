#pragma once

class KvExcitor;

// ������̵Ĺ�����

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