#pragma once
#include "KvCustomPlot.h"


class KcPlot1d : public KvCustomPlot
{
public:
	enum class KeType
	{
		k_scatter,  // 散点图
		k_line,   // 连线图
		k_bars,   // 柱状图
	};

	KcPlot1d(KvDataProvider* is, KeType type);

	kPropertySet propertySet() const override;

	void onPropertyChanged(int id, const QVariant& newVal) override;


	bool render(std::shared_ptr<KvData> data) override;

	void reset() override;

private:
	KpProperty scatterProperty_(bool hasNone) const;
	KpProperty lineProperty_(bool hasNone) const;
	KpProperty barProperty_() const;
	void updateBarWidth_();


protected:
	KeType type_;
	float barWidthRatio_; // 取值[0, 1]，为bin宽度的占比
};

