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

	unsigned subticks() const { return subticks_; }
	void setSubticks(unsigned count) { subticks_ = count; }

	std::string labelFormat() const { return format_; }
	void setLabelFormat(const std::string& fmt) { format_ = fmt; }

	// ���ؿ̶�ֵval��Ӧ��label��ȱʡʵ�ֵ���sprintf����val�ĸ�ʽ��
	virtual std::string label(double val) const;

	// ���ɲ�����[lower, upper]��������̶�����
	// @ticks:�������ɵĿ̶�������������0����ʹ��ticks_ֵ
	virtual std::vector<double> getTicks(double lower, double upper, unsigned ticks = 0) = 0;

	// ���ɲ��������и��̶����У��ṩȱʡʵ��
	// @subticks:�������ɵĸ��̶��������������̶�֮�䣩��������0����ʹ��subticks_ֵ
	virtual std::vector<double> getSubticks(const std::vector<double>& ticks, unsigned subticks = 0);

	// ������ֵ����[lower, upper]���������̶�����[lower, upper]
	// ȱʡʵ�ֲ���������ʼ�ս���ֵ�����lower��Ϊ���̶ȵ���ʼֵ
	virtual void autoRange(double& lower, double& upper) {}

protected:
	unsigned ticks_; // ���������̶�����
	unsigned subticks_; // �������̶�֮��ĸ��̶�����
	std::string format_; // ����tick-label�ĸ�ʽģ��
};
