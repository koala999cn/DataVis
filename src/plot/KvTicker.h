#pragma once
#include <vector>
#include <string>


// ������̶Ȳ��Եĳ����ࣺ��������������Ŀ̶�λ�úͿ̶�label

class KvTicker
{
public:
	KvTicker();
	virtual ~KvTicker();

	unsigned ticks() const { return ticks_; }
	void setTicks(unsigned count) { ticks_ = count; }

	std::string labelFormat() const { return format_; }
	void setLabelFormat(const std::string& fmt) { format_ = fmt; }

	// ���ؿ̶�ֵval��Ӧ��label��ȱʡʵ�ֵ���sprintf����val�ĸ�ʽ��
	virtual std::string label(double val) const;

	// ���ɲ�����[lower, upper]����Ŀ̶�����
	virtual std::vector<double> apply(double lower, double upper) = 0;

	// ������ֵ����[lower, upper]���������̶�����[lower, upper]
	// ȱʡʵ�ֲ���������ʼ�ս���ֵ�����lower��Ϊ���̶ȵ���ʼֵ
	virtual void autoRange(double& lower, double& upper) {}

protected:
	unsigned ticks_; // ���������̶�����
	std::string format_; // ����tick-label�ĸ�ʽģ��
};
