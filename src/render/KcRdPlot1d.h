#pragma once
#include "KvDataRender.h"
#include <memory>
#include <map>


class KcImPlot1d;
class KvPlottable;

class KcRdPlot1d : public KvDataRender
{
public:

	KcRdPlot1d();
	virtual ~KcRdPlot1d();

	// 根据输入构造plottables
	bool onNewLink(KcPortNode* from, KcPortNode* to) final;

	void onDelLink(KcPortNode* from, KcPortNode* to) final;

	bool onStartPipeline() final;

	void onInput(KcPortNode* outPort, unsigned inPort) final;

	void output() final {}

private:
	std::shared_ptr<KcImPlot1d> plot1d_;
	std::multimap<int, KvPlottable*> port2Plts_; // 端口id向plottable序列的映射（每个通道对应1个plottable）
	                                             // 1个端口可能有多个通道，为此可能映射到多个plottable
};

