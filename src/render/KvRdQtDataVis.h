#pragma once
#include "KvDataRender.h"

// ����Qt Data Visualizationʵ�ֵ���ά��ͼ�ӿ�

class QAbstract3DGraph;

class KvRdQtDataVis : public KvDataRender
{
public:
	KvRdQtDataVis(KvDataProvider* is, const QString& name);

	virtual ~KvRdQtDataVis();

	void setOption(KeObjectOption opt, bool on) override;

	bool getOption(KeObjectOption opt) const override;

protected:
	QAbstract3DGraph* graph3d_;
};