#pragma once
#include "KvCustomPlot.h"
#include "kDsp.h" // for kReal


class KcPlot2d : public KvCustomPlot
{
public:
	KcPlot2d(KvDataProvider* is);

	kPropertySet propertySet() const override;

	void reset() override;


private:
	void setPropertyImpl_(int id, const QVariant& newVal) override;
	bool renderImpl_(std::shared_ptr<KvData> data) override;
	void syncParent() override;

private:
	kReal dx_;
};

