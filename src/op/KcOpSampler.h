#pragma once
#include "KvDataOperator.h"
#include "KtSampling.h"
#include <vector>


class KcOpSampler : public KvDataOperator
{
public:
	KcOpSampler(KvDataProvider * prov);

	kPropertySet propertySet() const override;

	kRange range(kIndex axis) const override;

	kReal step(kIndex axis) const override;

	kIndex size(kIndex axis) const override;

	unsigned ins() const final { return 1u; }

	unsigned outs() const final { return 1u; }

private:
	void setPropertyImpl_(int id, const QVariant & newVal) override;

	void preRender_() override;

	std::shared_ptr<KvData> processImpl_(std::shared_ptr<KvData> data) override;

private:
	std::vector<KtSampling<kReal>> samps_;
};

