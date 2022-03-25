#pragma once
#include "KvCustomPlot.h"
#include "kDsp.h" // for kReal


class KcPlot2d : public KvCustomPlot
{
public:
	KcPlot2d(KvDataProvider* is);

	kPropertySet propertySet() const override;

	void onPropertyChanged(int id, const QVariant& newVal) override;

	bool render(std::shared_ptr<KvData> data) override;

	void reset() override;

private:
	kReal dx_;
};

