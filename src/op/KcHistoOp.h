#pragma once
#include "KvDataOperator.h"


class KcHistoOp : public KvDataOperator
{
public:
	KcHistoOp(KvDataProvider* prov);

	kPropertySet propertySet() const override;

	bool isStream() const override { return false; }

	kRange range(kIndex axis) const override;

	kReal step(kIndex axis) const override;

	unsigned ins() const final { return 1u; }

	unsigned outs() const final { return 1u; }


private:
	void setPropertyImpl_(int id, const QVariant& newVal) override;
	std::shared_ptr<KvData> processImpl_(std::shared_ptr<KvData> data) override;
	void syncParent() override {}

private:
	kReal xmin_, xmax_; // 统计区间
	unsigned bands_; // 
};

