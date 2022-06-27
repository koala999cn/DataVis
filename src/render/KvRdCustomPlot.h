#pragma once
#include "KvDataRender.h"
#include <QColor>


// 基于QCustomPlot实现的绘图类接口，主要实现以下功能：
// 1、创建QCustomPlot对象
// 2、提供background、axis等通用属性设置和处理
// 3、实现基类的show虚成员函数
// 4、缺省的上下文菜单及其处理
//

class QCustomPlot;
class KvDataProvider;
class KvData;


class KvRdCustomPlot : public KvDataRender
{
public:
	KvRdCustomPlot(KvDataProvider* is, const QString& name);
	virtual ~KvRdCustomPlot();

	kPropertySet propertySet() const override;

	void setOption(KeObjectOption opt, bool on) override;

	bool getOption(KeObjectOption opt) const override;

	QString exportAs();

	void showData();

	virtual void rescaleAxes();


protected:
	void setPropertyImpl_(int id, const QVariant& newVal) override;


protected:
	std::shared_ptr<KvData> contData_; // 最近接收的连续数据，由派生类维护使用

	QCustomPlot* customPlot_;

	// QCustomPlot有关属性
	bool autoScale_; // 是否自动缩放axis适应输入数据 
	QColor back_;
};

