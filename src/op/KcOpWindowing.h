#pragma once
#include "KvDataOperator.h"

class KgWindowing;

class KcOpWindowing : public KvDataOperator
{
public:
	KcOpWindowing(KvDataProvider* prov);

	kPropertySet propertySet() const override;

	unsigned ins() const final { return 1u; }

	unsigned outs() const final { return 1u; }

private:
	void setPropertyImpl_(int id, const QVariant& newVal) override;

	void syncParent() override;

	std::shared_ptr<KvData> processImpl_(std::shared_ptr<KvData> data) override;

private:
	std::unique_ptr<KgWindowing> win_;
	int type_;
	kReal arg_;
};

