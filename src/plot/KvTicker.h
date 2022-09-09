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

	std::string labelFormat() const { return format_; }
	void setLabelFormat(const std::string& fmt) { format_ = fmt; }

	// 返回刻度值val对应的label，缺省实现调用sprintf返回val的格式串
	virtual std::string label(double val) const;

	// 生成并返回[lower, upper]区间的刻度序列
	virtual std::vector<double> apply(double lower, double upper) = 0;

	// 输入数值区间[lower, upper]，返回主刻度区间[lower, upper]
	// 缺省实现不作调整，始终将数值区间的lower作为主刻度的起始值
	virtual void autoRange(double& lower, double& upper) {}

protected:
	unsigned ticks_; // 期望的主刻度数量
	std::string format_; // 生成tick-label的格式模板
};
