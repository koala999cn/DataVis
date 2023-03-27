#pragma once
#include "KvDataRender.h"
#include <memory>
#include <map>


class KvPlot;
class KcAxis;
class KcCoordPlane;
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

	void showPropertySet() override;

	void onDoubleClicked() override;

	virtual void onThemeChanged(int type);

protected:

	// 一个数据源可以创建多个KvPlottable
	virtual std::vector<KvPlottable*> createPlottable_(KcPortNode* port) = 0;
	
	// 创建plottable的帮助函数，对外提供splitChannels_无关的接口
	// 用户在createPlottable_接口实现中可调用
	template<typename PLT_TYPE>
	std::vector<KvPlottable*> createPlts_(KcPortNode* port) {
		auto prov = std::dynamic_pointer_cast<KvDataProvider>(port->parent().lock());
		if (prov->channels(port->index()) == 1 || !splitChannels_)
			return { new PLT_TYPE(prov->name()) };

		std::vector<KvPlottable*> plts;
		plts.resize(prov->channels(port->index()));

		for (kIndex ch = 0; ch < prov->channels(port->index()); ch++) {
			std::string name = prov->name() + " - ch" + std::to_string(ch);
			plts[ch] = new PLT_TYPE(name);
		}

		return plts;
	}


	/// 属性显示的子接口

	virtual void showPlotProperty_();

	virtual void showThemeProperty_();

	virtual void showCoordProperty_();

	    virtual void showAxisProperty_(KcAxis&);

		virtual void showPlaneProperty_(KcCoordPlane&);

	virtual void showLegendProperty_();
	
	virtual void showColorBarProperty_();

	virtual void showPlottableProperty_();

	    // 返回true表示用户更改了type
	    bool showPlottableBasicProperty_(unsigned idx, KvPlottable*);

		void showPlottableArrangeProperty_(KvPlottable*);

		void showPlottableColoringProperty_(KvPlottable*);

		void showPlottableDefaultZProperty_(KvPlottable*);

	    virtual void showPlottableSpecificProperty_(KvPlottable*) {};

	////////////////////////////////////////////////////


	/// plottable类型转换相关接口，由showPlottableTypeProperty_方法调用

	// 返回支持的plottable类型数
	virtual unsigned supportPlottableTypes_() const = 0;

	// 返回plt的类型
	virtual int plottableType_(KvPlottable* plt) const = 0;

	// 返回第iType类型的字符串
	virtual const char* plottableTypeStr_(int iType) const = 0;

	// 创建第iType类型的plottable
	virtual KvPlottable* newPlottable_(int iType, const std::string& name) = 0;

	// 第iType类型的plottable是否支持KvData数据
	virtual bool plottableMatchData_(int iType, const KvData& d) const { return true; }

	////////////////////////////////////////////////////


	void updateTheme_();

	using data_ptr = std::shared_ptr<KvData>;
	static data_ptr streaming_(data_ptr curData, data_ptr newData, unsigned nx);

	// 根据inputs的range，自动设置plot的坐标轴extend
	void autoRange_();

protected:
	std::shared_ptr<KvPlot> plot_;
	std::multimap<int, KvPlottable*> port2Plts_; // 端口id向plottable序列的映射
												 // 1个端口可能有多个通道，为此可能映射到多个plottable（每个通道对应1个plottable）
	
	// 对于stream输入，创建1个数据副本. 因为输入数据是动态的，绘图的时候会出现抖动
	std::map<KcPortNode*, std::shared_ptr<KvData>> streamData_;

	std::pair<std::string, std::string> curTheme_[4];

	bool splitChannels_{ false }; // 多通道数据是创建1个还是多个plt？ 

};
