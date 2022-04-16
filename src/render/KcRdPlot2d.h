#pragma once
#include "KvRdCustomPlot.h"
#include "kDsp.h" // for kReal


class QCPColorScale;

class KcRdPlot2d : public KvRdCustomPlot
{
public:
	KcRdPlot2d(KvDataProvider* is);

	kPropertySet propertySet() const override;

	void reset() override;


private:
	void setPropertyImpl_(int id, const QVariant& newVal) override;
	bool renderImpl_(std::shared_ptr<KvData> data) override;
	void syncParent() override;

private:
	kReal dx_;
	QCPColorScale* colorScale_;
};

