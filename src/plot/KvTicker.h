#pragma once
#include <vector>
#include <string>


// 坐标轴刻度策略的抽象类：用于生成坐标轴的刻度位置和刻度label

class KvTicker
{
public:

	unsigned ticksExpected() const { return ticksExpected_; }
	unsigned& ticksExpected() { return ticksExpected_; }

	unsigned subticksExpected() const { return subticksExpected_; }
	unsigned& subticksExpected() { return subticksExpected_; }
	
	// 将数据val变换到ticker所在的值域，如log域
	// NB: 输入为linear域
	virtual double map(double val) const { return val; }

	// 根据传入的参数生成刻度
	virtual void update(double lower, double upper, bool skipSubticks) = 0;

	virtual unsigned ticksTotal() const = 0;

	// NB: 该函数返回subticks的总数，而非两个主刻度之间的subticks数
	virtual unsigned subticksTotal() const = 0;

	virtual double tick(unsigned idx) const = 0;

	virtual double subtick(unsigned idx) const = 0;

	virtual std::string label(unsigned idx) const = 0;

private:
	unsigned ticksExpected_{ 5 }; // 期望的主刻度数量
	unsigned subticksExpected_{ 4 }; // 期望的两个主刻度之间的副刻度数量
};
