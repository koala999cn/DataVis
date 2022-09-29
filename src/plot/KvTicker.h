#pragma once
#include <vector>
#include <string>


// 坐标轴刻度策略的抽象类：用于生成坐标轴的刻度位置和刻度label

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

	// 返回刻度值val对应的label，缺省实现调用sprintf返回val的格式串
	virtual std::string label(double val) const;

	// 生成并返回[lower, upper]区间的主刻度序列
	// @ticks:期望生成的刻度数量，若等于0，则使用ticks_值
	virtual std::vector<double> getTicks(double lower, double upper, unsigned ticks = 0) = 0;

	// 生成并返回所有副刻度序列，提供缺省实现
	// @subticks:期望生成的副刻度数量（两个主刻度之间），若等于0，则使用subticks_值
	virtual std::vector<double> getSubticks(const std::vector<double>& ticks, unsigned subticks = 0);

	// 输入数值区间[lower, upper]，返回主刻度区间[lower, upper]
	// 缺省实现不作调整，始终将数值区间的lower作为主刻度的起始值
	virtual void autoRange(double& lower, double& upper) {}

protected:
	unsigned ticks_; // 期望的主刻度数量
	unsigned subticks_; // 两个主刻度之间的副刻度数量
	std::string format_; // 生成tick-label的格式模板
};
