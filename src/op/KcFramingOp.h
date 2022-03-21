#pragma once
#include "KvDataOperator.h"


class KcFramingOp : public KvDataOperator
{
public:
	KcFramingOp(KvDataProvider* prov);
	virtual ~KcFramingOp();

	kPropertySet propertySet() const override;

	void onPropertyChanged(int id, const QVariant& newVal) override;

	bool isStream() const override { return true; }

	unsigned dim() const override { return 2; }

	kRange range(kIndex axis) const override;

	kReal step(kIndex axis) const override;

	unsigned ins() const final { return 1u; }

	unsigned outs() const final { return 2u; }

private:
	std::shared_ptr<KvData> processImpl_(std::shared_ptr<KvData> data) override;

private:
	void* d_ptr_;
};

