#pragma once
#include "KvDataOperator.h"
#include <memory>

class KgFraming;

class KcFramingOp : public KvDataOperator
{
public:
	KcFramingOp(KvDataProvider* prov);

	kPropertySet propertySet() const override;

	bool isStream() const override { return true; }

	kIndex dim() const override;

	kRange range(kIndex axis) const override;

	kReal step(kIndex axis) const override;

	kIndex length(kIndex axis) const override;

	unsigned ins() const final { return 1u; }

	unsigned outs() const final { return 2u; }


private:
	void setPropertyImpl_(int id, const QVariant& newVal) override;

	void syncParent() override;

	std::shared_ptr<KvData> processImpl_(std::shared_ptr<KvData> data) override;

private:
	std::unique_ptr<KgFraming> framing_;
	kIndex channels_;
	kReal dx_;
};

