#pragma once
#include <vector>
#include <string>


// ������̶Ȳ��Եĳ����ࣺ��������������Ŀ̶�λ�úͿ̶�label

class KvAxisTicker
{
public:
	KvAxisTicker();
	virtual ~KvAxisTicker();

	unsigned tickCount() const { return tickCount_; }
	void setTickCount(unsigned count);

	// ���ɲ�����[lower, upper]��������̶�����
	virtual std::vector<double> generate(double lower, double upper, std::vector<double>* subTicks, std::vector<std::string>* tickLabels);


private:
	unsigned tickCount_;
};
