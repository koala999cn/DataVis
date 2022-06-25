#pragma once


#pragma once
#include "KvRdPlot3d.h"


class KcRdSurface3d : public KvRdPlot3d
{
public:
	KcRdSurface3d(KvDataProvider* is);
	virtual ~KcRdSurface3d();

	kPropertySet propertySet() const override;

	void reset() override;

private:
	void setPropertyImpl_(int id, const QVariant& newVal) override;
	bool renderImpl_(std::shared_ptr<KvData> data) override;
	void syncParent() override;
};
