#pragma once
#include "KvDataRender.h"
#include <memory>
#include <map>


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

	void onDoubleClicked() override;

	virtual void onThemeChanged(int type);

protected:

	// 一个数据源可以创建多个KvPlottable
	virtual std::vector<KvPlottable*> createPlottable_(KvDataProvider* prov) = 0;

	virtual void showPlottableProperty_(unsigned idx) {};

	void showThemeProperty_();

	// 创建plottable的帮助函数，对外提供splitChannels_无关的接口
	// 用户在createPlottable_接口实现中可调用
	template<typename PLT_TYPE>
	std::vector<KvPlottable*> createPlts_(KvDataProvider* prov) {
		if (prov->channels() == 1)
			return { new PLT_TYPE(prov->name()) };

		std::vector<KvPlottable*> plts;
		plts.resize(prov->channels());

		for (kIndex ch = 0; ch < prov->channels(); ch++) {
			std::string name = prov->name() + " - ch" + KuStrUtil::toString(ch);
			plts[ch] = new PLT_TYPE(name);
		}

		return plts;
	}

	void updateTheme_();

protected:
	std::shared_ptr<KvPlot> plot_;
	std::multimap<int, KvPlottable*> port2Plts_; // 端口id向plottable序列的映射
												 // 1个端口可能有多个通道，为此可能映射到多个plottable（每个通道对应1个plottable）
	
	// 对于stream输入，创建1个数据副本. 因为输入数据是动态的，绘图的时候会出现抖动
	std::map<KcPortNode*, std::shared_ptr<KvData>> streamData_;

	std::pair<std::string, std::string> curTheme_[4];

	bool splitChannels_{ false }; // 多通道数据是创建1个还是多个plt？ 
};
