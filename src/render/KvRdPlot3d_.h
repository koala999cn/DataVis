#pragma once
#include "KvDataRender.h"

// ����VLʵ�ֵ���ά��ͼ�ӿ�
// �ṩͨ�õ����Կ���

class KvData;

class KvRdPlot3d_ : public KvDataRender
{
	Q_OBJECT

public:
	KvRdPlot3d_(KvDataProvider* is, const QString& name);

	virtual ~KvRdPlot3d_();

	void setOption(KeObjectOption opt, bool on) override;

	bool getOption(KeObjectOption opt) const override;

	kPropertySet propertySet() const override;


public slots:
	void setSize(int axis, int newSize);


signals:
	void sizeChanged(int axis, int newSize); // ֻ����Ⱦ��������ʱ���ŷ��ʹ��ź�


protected:
	void setPropertyImpl_(int id, const QVariant& newVal) override;;

	// ���ո��ڵ��range�������ø���range
	void syncAxes_();

protected:
	std::shared_ptr<KvData> contData_; // ������յ��������ݣ���������ά��ʹ��
};
