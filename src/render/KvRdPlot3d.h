#pragma once
#include "KvDataRender.h"

// ����Qt Data Visualizationʵ�ֵ���ά��ͼ�ӿ�
// �ṩͨ�õ����Կ���

class QAbstract3DGraph;
class QAbstract3DAxis;

class KvRdPlot3d : public KvDataRender
{
public:
	KvRdPlot3d(KvDataProvider* is, const QString& name);

	virtual ~KvRdPlot3d();

	void setOption(KeObjectOption opt, bool on) override;

	bool getOption(KeObjectOption opt) const override;

	kPropertySet propertySet() const override;


public slots:
	void setSize(int axis, int newSize);


signals:
	void sizeChanged(int axis, int newSize) {} // ֻ����Ⱦ��������ʱ���ŷ��ʹ��ź�


protected:
	void setPropertyImpl_(int id, const QVariant& newVal) override;;

	// ���ո��ڵ��range�������ø���range
	void syncAxes_();

protected:
	QWidget* widget_; // ��ʱ������QWindow����

	// �������ڹ��캯���г�ʼ�����³�Ա
	QAbstract3DGraph* graph3d_;
	QAbstract3DAxis *xAxis_, *yAxis_, *zAxis_; // �ٶ�x�����ң�y�����⣬z������
	int size0_, size1_;
};
