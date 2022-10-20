#pragma once
#include "KvDataRender.h"
#include <memory>
#include <map>
#include <vector>


class KvPlot;
class KvPlottable;
class KvDataProvider;
class KvThemedPlot;

class KvRdPlot : public KvDataRender
{
public:

	using super_ = KvDataRender;

	KvRdPlot(const std::string_view& name, const std::shared_ptr<KvPlot>& plot);
	virtual ~KvRdPlot();

	// 根据输入构造plottables
	bool onNewLink(KcPortNode* from, KcPortNode* to) override;

	void onDelLink(KcPortNode* from, KcPortNode* to) override;

	bool onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins) override;

	void onInput(KcPortNode* outPort, unsigned inPort) override;

	void output() override {}

	void showProperySet() override;


protected:

	// 一个数据源可以创建多个KvPlottable
	virtual std::vector<KvPlottable*> createPlottable_(KvDataProvider* prov) = 0;

	void showThemeProperty_();

	// 在应用theme的时候，同步包含的canvas, layout, palette等主题选项
	void applyTheme_(const std::string& name, KvThemedPlot* plot);

protected:
	std::shared_ptr<KvPlot> plot_;
	std::multimap<int, KvPlottable*> port2Plts_; // 端口id向plottable序列的映射
												 // 1个端口可能有多个通道，为此可能映射到多个plottable（每个通道对应1个plottable）
	
	std::string themeName_, canvasName_, layoutName_, paletteName_;
};
