#pragma once
#include <vector>
#include <string>


// ������̶Ȳ��Եĳ����ࣺ��������������Ŀ̶�λ�úͿ̶�label

class KvTicker
{
public:
	KvTicker();
	virtual ~KvTicker();

	unsigned tickCount() const { return tickCount_; }
	void setTickCount(unsigned count) { tickCount_ = count; }

	unsigned subtickCount() const { return subtickCount_; }
	void setSubtickCount(unsigned count) { subtickCount_ = count; }

	std::string labelFormat() const { return format_; }
	void setLabelFormat(const std::string& fmt) { format_ = fmt; }

	// ���ɿ̶ȣ����ݴ���Ĳ������ticks_, subticks_, labels_��Ա
	virtual void generate(double lower, double upper, bool genSubticks, bool genLabels) = 0;

	const std::vector<double>& ticks() const { return ticks_; }

	const std::vector<double>& subticks() const { return subticks_; }

	const std::vector<std::string>& labels() const { return labels_; }

protected:

	// ���ؿ̶�ֵval��Ӧ��label��ȱʡʵ�ֵ���sprintf����val�ĸ�ʽ��
	virtual std::string genLabel_(double val) const;

protected:
	unsigned tickCount_; // ���������̶�����
	unsigned subtickCount_; // �������������̶�֮��ĸ��̶�����
	std::string format_; // ����tick-label�ĸ�ʽģ��

	// ����Ϊ���ɵĿ̶�ֵ��label�ı�
	std::vector<double> ticks_;
	std::vector<double> subticks_;
	std::vector<std::string> labels_;
};
