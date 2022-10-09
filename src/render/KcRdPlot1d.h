#pragma once
#include "KvDataRender.h"
#include <memory>

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

	bool onStartPipeline() final;

	void onInput(KcPortNode* outPort, unsigned inPort) final;

	void output() final {}

private:
	KeType type_;
	std::shared_ptr<KcImPlot1d> plot1d_;
};

