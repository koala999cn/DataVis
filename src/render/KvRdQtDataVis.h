#pragma once
#include "KvDataRender.h"

// ����Qt Data Visualizationʵ�ֵ���ά��ͼ�ӿ�
// �ṩͨ�õ����Կ���

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