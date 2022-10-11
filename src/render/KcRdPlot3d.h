#pragma once
#include "KvDataRender.h"
#include <memory>
#include <map>

class KcImPlot3d;
class KvPlottable;

class KcRdPlot3d : public KvDataRender
{
public:

	using super_ = KvDataRender;

	KcRdPlot3d();
	virtual ~KcRdPlot3d();

	// 根据输入构造plottables
	bool onNewLink(KcPortNode* from, KcPortNode* to) final;

	void onDelLink(KcPortNode* from, KcPortNode* to) final;

	bool onStartPipeline() final;

	void onInput(KcPortNode* outPort, unsigned inPort) final;

	void output() final {}

	void showProperySet() final;

private:
	std::shared_ptr<KcImPlot3d> plot3d_;
	std::multimap<int, KvPlottable*> port2Plts_; // 端口id向plottable序列的映射
												 // 1个端口可能有多个通道，为此可能映射到多个plottable（每个通道对应1个plottable）
};

