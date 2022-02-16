#pragma once

#define QCUSTOMPLOT_USE_OPENGL
#define QCUSTOMPLOT_USE_LIBRARY
#include "qcustomplot/qcustomplot.h"
#include "KvOutputDevice.h"
#include <QObject>
#include <QStringList>
#include <QColor>
#include "kddockwidgets/DockWidget.h"
#include "kddockwidgets/MainWindow.h"

class KvInputSource;
class KvData;

class KcDockPlotter : public KvOutputDevice
{
	Q_OBJECT 

public:
	enum KePlotType
	{
		k_point_line,  // 点线图
		k_bar,   // 柱状图
	};

	KcDockPlotter(KvInputSource* is, KDDockWidgets::MainWindow* dockFrame);
	virtual ~KcDockPlotter();

	int type() const { return type_; }
	void setType(int type);

	// 重载该函数以同步更新dock标题
	void setName(const QString& newName) override;

	kPropertySet propertySet() const override;

	void onPropertyChanged(int id, const QVariant& newVal) override;

	bool show();

	// 将input-source中的数据同步到plot-widget
	void syncData();

	void streamData(std::shared_ptr<KvData> data);

	// 导出
	QString exportPlot();

	// 返回支持的绘图类型数
	static int supportedTypes();

	static QStringList supportedTypeStrs();

	// 根据类型id返回文字表述
	static QString typeToStr(int type);

	// 根据文字表述返回类型id
	static int typeFromStr(const QString& str);

protected:
	KDDockWidgets::DockWidgetBase* getDock_(bool createWhenNeeded = false);
	void clearData_(QCPAbstractPlottable* p);

private:
	int type_;
	KvInputSource* input_;
	QCustomPlot* customPlot_;
	KDDockWidgets::MainWindow* dockFrame_;

	// QCustomPlot有关属性
	bool autoScale_; // 是否自动缩放axis适应输入数据 
	QColor back_;
};

