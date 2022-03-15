#pragma once
#include "KvDataOperator.h"


class KcSpectrogramOp : public KvDataOperator
{
public:
	KcSpectrogramOp(KvDataProvider* prov);

	kPropertySet propertySet() const override;

	void onPropertyChanged(int id, const QVariant& newVal) override;

	unsigned ins() const final { return 2u; }

	unsigned outs() const final { return 2u; }

private:
	std::shared_ptr<KvData> processImpl_(std::shared_ptr<KvData> data) override;
};

