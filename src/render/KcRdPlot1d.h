#pragma once
#include "KvDataRender.h"
#include <memory>
#include <unordered_map>


class KcImPlot1d;

class KcRdPlot1d : public KvDataRender
{
public:

	// 支持的6种一维图类型. TODO: 混合类型的支持
	enum KeType
	{
		k_scatter,  // 散点图
		k_line,   // 连线图
		k_line_scatter, // 点线图
		k_line_fill, // 填充图
		k_bars_stacked,   // 层叠柱状图
		k_bars_grouped, // 分组柱状图
		k_type_count
	};

	KcRdPlot1d();
	virtual ~KcRdPlot1d();

	// 根据输入构造plottables
	bool onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins) final;

	void onInput(KcPortNode* outPort, unsigned inPort) final;

	void output() final {}

private:
	KeType type_;
	std::shared_ptr<KcImPlot1d> plot1d_;
	std::unordered_map<int, unsigned> portId2Idx_; // 端口id向plottable序号的映射
};

