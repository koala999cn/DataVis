#pragma once
#include "KvDataRender.h"

// ����Qt Data Visualizationʵ�ֵ���ά��ͼ�ӿ�
// �ṩͨ�õ����Կ���

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

	// ���ո��ڵ��range�������ø���range
	void syncAxes_();

protected:
	// �������ڹ��캯���г�ʼ�����³�Ա
	QAbstract3DGraph* graph3d_;
	QAbstract3DAxis *xAxis_, *yAxis_, *zAxis_;
	QWidget* widget_;
};