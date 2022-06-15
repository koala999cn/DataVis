#pragma once
#include "KvDataRender.h"

// 基于Qt Data Visualization实现的三维绘图接口
// 提供通用的属性控制

class QAbstract3DGraph;

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

protected:
	QAbstract3DGraph* graph3d_;
	int theme_;
};