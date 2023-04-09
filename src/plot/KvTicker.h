#pragma once
#include <vector>
#include <string>


// ������̶Ȳ��Եĳ����ࣺ��������������Ŀ̶�λ�úͿ̶�label

class KvTicker
{
public:

	unsigned ticksExpected() const { return ticksExpected_; }
	unsigned& ticksExpected() { return ticksExpected_; }

	unsigned subticksExpected() const { return subticksExpected_; }
	unsigned& subticksExpected() { return subticksExpected_; }
	
	// ������val�任��ticker���ڵ�ֵ����log��
	// NB: ����Ϊlinear��
	virtual double map(double val) const { return val; }

	// ���ݴ���Ĳ������ɿ̶�
	virtual void update(double lower, double upper, bool skipSubticks) = 0;

	virtual unsigned ticksTotal() const = 0;

	// NB: �ú�������subticks�������������������̶�֮���subticks��
	virtual unsigned subticksTotal() const = 0;

	virtual double tick(unsigned idx) const = 0;

	virtual double subtick(unsigned idx) const = 0;

	virtual std::string label(unsigned idx) const = 0;

private:
	unsigned ticksExpected_{ 5 }; // ���������̶�����
	unsigned subticksExpected_{ 4 }; // �������������̶�֮��ĸ��̶�����
};
