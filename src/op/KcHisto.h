#pragma once
#include "KvDataOperator.h"


class KcHisto : public KvDataOperator
{
public:
	KcHisto(KvDataProvider* prov);

	kPropertySet propertySet() const override;

	void onPropertyChanged(int id, const QVariant& newVal) override;

	bool isStream() const override { return false; }

	kRange range(kIndex axis) const override;

	kReal step(kIndex axis) const override;

	unsigned ins() const final { return 1u; }

	unsigned outs() const final { return 1u; }

private:
	std::shared_ptr<KvData> processImpl_(std::shared_ptr<KvData> data) override;

	kReal xmin_, xmax_; // 统计区间
	unsigned bands_; // 
};

