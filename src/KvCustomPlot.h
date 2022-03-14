﻿#pragma once
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


class KvCustomPlot : public KvDataRender
{
public:
	KvCustomPlot(KvDataProvider* is, const QString& name);
	virtual ~KvCustomPlot();

	kPropertySet propertySet() const override;

	void onPropertyChanged(int id, const QVariant& newVal) override;


	bool canShown() const override;

	bool isVisible() const override;

	void show(bool bShow) override;


	// 导出
	QString exportAs();

	virtual void rescaleAxes();

protected:
	QCustomPlot* customPlot_;

	// QCustomPlot有关属性
	bool autoScale_; // 是否自动缩放axis适应输入数据 
	QColor back_;
};
