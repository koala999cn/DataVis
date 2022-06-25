#pragma once
#include "KvRdPlot3d.h"


class KcRdScatter3d : public KvRdPlot3d
{
public:
	KcRdScatter3d(KvDataProvider* is);
	virtual ~KcRdScatter3d();

	kPropertySet propertySet() const override;

	void reset() override;

private:
	void setPropertyImpl_(int id, const QVariant& newVal) override;
	bool renderImpl_(std::shared_ptr<KvData> data) override;
	void syncParent() override;

	void syncAxes_();
};
