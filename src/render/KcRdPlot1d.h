#pragma once
#include "KvRdCustomPlot.h"


class KcRdPlot1d : public KvRdCustomPlot
{
public:
	enum class KeType
	{
		k_scatter,  // 散点图
		k_line,   // 连线图
		k_bars,   // 柱状图
	};

	KcRdPlot1d(KvDataProvider* is, KeType type);

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


protected:
	KeType type_;
	float barWidthRatio_; // 取值[0, 1]，为bin宽度的占比
};

