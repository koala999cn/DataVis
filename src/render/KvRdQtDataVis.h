#pragma once
#include "KvDataRender.h"

// 基于Qt Data Visualization实现的三维绘图接口
// 提供通用的属性控制

class QAbstract3DGraph;
class QAbstract3DAxis;

class KvRdQtDataVis : public KvDataRender
{
public:
	KvRdQtDataVis(KvDataProvider* is, const QString& name);

	virtual ~KvRdQtDataVis();

	void setOption(KeObjectOption opt, bool on) override;

	bool getOption(KeObjectOption opt) const override;

	kPropertySet propertySet() const override;


protected:
	void setPropertyImpl_(int id, const QVariant& newVal) override;;

	// 按照父节点的range属性设置各轴range
	void syncAxes_();

protected:
	// 子类须在构造函数中初始化以下成员
	QAbstract3DGraph* graph3d_;
	QAbstract3DAxis *xAxis_, *yAxis_, *zAxis_;
	QWidget* widget_;
};