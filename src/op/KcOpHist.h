#pragma once
#include "KvDataOperator.h"
#include <memory>


class KgHist;

class KcOpHist : public KvDataOperator
{
public:
	KcOpHist(KvDataProvider* prov);

	kPropertySet propertySet() const override;

	bool isStream() const override { return false; }

	kRange range(kIndex axis) const override;

	kReal step(kIndex axis) const override;

	unsigned ins() const final { return 1u; }

	unsigned outs() const final { return 1u; }


private:
	void setPropertyImpl_(int id, const QVariant& newVal) override;
	std::shared_ptr<KvData> processImpl_(std::shared_ptr<KvData> data) override;
	void syncParent() override;

private:
	std::unique_ptr<KgHist> hist_;
};

