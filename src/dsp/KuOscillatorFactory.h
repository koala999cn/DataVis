#pragma once
#include "KvExcitor.h"
#include "rchoscillators/rch_oscillators.h"


// ��������

class KuOscillatorFactory
{
public:

	// ����֧�ֵ���������
	static unsigned typeCount();

	// ���ص�idx������������
	static const char* typeName(unsigned typeIdx);

	// ͨ���������ִ���
	static KvExcitor* create(const char* name);

	// ͨ���������ʹ���
	static KvExcitor* create(unsigned typeIdx);

private:
	KuOscillatorFactory() = delete;
};
