#pragma once
#include "KvRdQtDataVis.h"

class QBar3DSeries;

class KcRdBar3d : public KvRdQtDataVis
{
public:
	KcRdBar3d(KvDataProvider* is);
	virtual ~KcRdBar3d();

	kPropertySet propertySet() const override;

	void reset() override;

private:
	void setPropertyImpl_(int id, const QVariant& newVal) override;
	bool renderImpl_(std::shared_ptr<KvData> data) override;
	void syncParent() override;

private:
	QBar3DSeries* series_;
};
