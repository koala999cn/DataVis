#pragma once
#include "KvRdCustomPlot.h"


class KcRdPlot1d : public KvRdCustomPlot
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

	// @type: 初始类型
	KcRdPlot1d(KvDataProvider* is, KeType type = k_line);

	kPropertySet propertySet() const override;

	void reset() override;


private:
	void setPropertyImpl_(int id, const QVariant& newVal) override;
	bool doRender_(std::shared_ptr<KvData> data) override;
	void preRender_() override;

	KpProperty scatterProperty_(bool hasNone) const;
	KpProperty lineProperty_(bool hasNone) const;
	KpProperty barProperty_() const;
	void updateBarWidth_();
	
	// 根据type_，构建指定类型的图元素
	void createTypedPlot_();

protected:
	KeType type_;
	bool delayedCreate_; // 是否延迟调用createTypedPlot_，以确保同步doRender_
	float barWidthRatio_; // 取值[0, 1]，为bin宽度的占比
};

