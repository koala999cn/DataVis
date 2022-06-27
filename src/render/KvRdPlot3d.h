#pragma once
#include "KvDataRender.h"

// 基于Qt Data Visualization实现的三维绘图接口
// 提供通用的属性控制

class QAbstract3DGraph;
class QAbstract3DAxis;
class KvData;

class KvRdPlot3d : public KvDataRender
{
	Q_OBJECT

public:
	KvRdPlot3d(KvDataProvider* is, const QString& name);

	virtual ~KvRdPlot3d();

	void setOption(KeObjectOption opt, bool on) override;

	bool getOption(KeObjectOption opt) const override;

	kPropertySet propertySet() const override;


public slots:
	void setSize(int axis, int newSize);


signals:
	void sizeChanged(int axis, int newSize); // 只有渲染连续数据时，才发送此信号


protected:
	void setPropertyImpl_(int id, const QVariant& newVal) override;;

	// 按照父节点的range属性设置各轴range
	void syncAxes_();

protected:
	std::shared_ptr<KvData> contData_; // 最近接收的连续数据，由派生类维护使用
	QWidget* widget_; // 临时创建的QWindow容器

	// 子类须在构造函数中初始化以下成员
	QAbstract3DGraph* graph3d_;
	QAbstract3DAxis *xAxis_, *yAxis_, *zAxis_; // 假定x轴向右，y轴向外，z轴向上
	int size0_, size1_;
};
