#pragma once
#include "KvRdQwtPlot3d.h"


class KcRdGrid3d : public KvRdQwtPlot3d
{
public:
	KcRdGrid3d(KvDataProvider* is);
	virtual ~KcRdGrid3d();

	kPropertySet propertySet() const override;

	void reset() override;

private:
	void setPropertyImpl_(int id, const QVariant& newVal) override;
	bool doRender_(std::shared_ptr<KvData> data) override;
	void preRender_() override;
};