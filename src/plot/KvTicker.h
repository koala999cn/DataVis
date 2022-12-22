#pragma once
#include <vector>
#include <string>


// 坐标轴刻度策略的抽象类：用于生成坐标轴的刻度位置和刻度label

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

	// 生成刻度，根据传入的参数填充ticks_, subticks_, labels_成员
	virtual void generate(double lower, double upper, bool genSubticks, bool genLabels) = 0;

	const std::vector<double>& ticks() const { return ticks_; }

	const std::vector<double>& subticks() const { return subticks_; }

	const std::vector<std::string>& labels() const { return labels_; }

protected:

	// 返回刻度值val对应的label，缺省实现调用sprintf返回val的格式串
	virtual std::string genLabel_(double val) const;

protected:
	unsigned tickCount_; // 期望的主刻度数量
	unsigned subtickCount_; // 期望的两个主刻度之间的副刻度数量
	std::string format_; // 生成tick-label的格式模板

	// 以下为生成的刻度值与label文本
	std::vector<double> ticks_;
	std::vector<double> subticks_;
	std::vector<std::string> labels_;
};
