#pragma once

class KvOscillator;

// �����ź������Ĺ�����

class KuOscillatorFactory
{
public:

	// ����֧�ֵ���������
	static unsigned typeCount();

	// ���ص�idx������������
	static const char* typeName(unsigned typeIdx);

	// ͨ���������ִ���
	static KvOscillator* create(const char* name);

	// ͨ���������ʹ���
	static KvOscillator* create(unsigned typeIdx);

private:
	KuOscillatorFactory() = delete;
};
