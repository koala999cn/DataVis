#pragma once
#include <vector>
#include <string>


// 坐标轴刻度策略的抽象类：用于生成坐标轴的刻度位置和刻度label

class KvAxisTicker
{
public:
	KvAxisTicker();
	virtual ~KvAxisTicker();

	unsigned tickCount() const { return tickCount_; }
	void setTickCount(unsigned count);

	// 生成并返回[lower, upper]区间的主刻度序列
	virtual std::vector<double> generate(double lower, double upper, std::vector<double>* subTicks, std::vector<std::string>* tickLabels);


private:
	unsigned tickCount_;
};
