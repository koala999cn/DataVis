#pragma once
#include "KvDataOperator.h"


class KcSpectrogramOp : public KvDataOperator
{
public:
	KcSpectrogramOp(KvDataProvider* prov);

	kPropertySet propertySet() const override;

	unsigned ins() const final { return 2u; }

	unsigned outs() const final { return 2u; }

private:
	void setPropertyImpl_(int id, const QVariant& newVal) override;

	std::shared_ptr<KvData> processImpl_(std::shared_ptr<KvData> data) override;
};

