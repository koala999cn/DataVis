#pragma once
#include "KvDataRender.h"
#include <QObject>
#include <QStringList>
#include <QColor>

class QCustomPlot;
class KvDataProvider;
class KvData;

class KcDataPlot : public KvDataRender
{
	Q_OBJECT 

public:
	enum class KePlot1dType
	{
		k_scatter,  // 散点图
		k_line,   // 连线图
		k_bars,   // 柱状图
	};

	KcDataPlot(KvDataProvider* is, KePlot1dType type);
	virtual ~KcDataPlot();

	KePlot1dType type() const { return type_; }

	kPropertySet propertySet() const override;

	void onPropertyChanged(int id, const QVariant& newVal) override;


	bool render(std::shared_ptr<KvData> data) override;

	// 导出
	QString exportAs();

protected:
	virtual void rescaleAxes();

private:
	KpProperty scatterProperty_(bool hasNone) const;
	KpProperty lineProperty_(bool hasNone) const;
	KpProperty barProperty_() const;
	KpProperty axisProperty_() const;

private:
	KePlot1dType type_;
	QCustomPlot* customPlot_;

	// QCustomPlot有关属性
	bool autoScale_; // 是否自动缩放axis适应输入数据 
	QColor back_;
};

