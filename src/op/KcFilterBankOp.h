#pragma once
#include "KvDataOperator.h"
#include <memory>

class KgFilterBank;

class KcFilterBankOp : public KvDataOperator
{
public:
	KcFilterBankOp(KvDataProvider* prov);

	bool isStream() const override { return false; }

	//kIndex dim() const override;

	kRange range(kIndex axis) const override;

	kReal step(kIndex axis) const override;

	kIndex length(kIndex axis) const override;

	kPropertySet propertySet() const override;

	unsigned ins() const final { return 1u; }

	unsigned outs() const final { return 1u; }

private:
	void setPropertyImpl_(int id, const QVariant& newVal) override;

	std::shared_ptr<KvData> processImpl_(std::shared_ptr<KvData> data) override;

	void syncParent() override;

	std::shared_ptr<KvData> process2d_(std::shared_ptr<KvData> data);

private:
	std::unique_ptr<KgFilterBank> fbank_;

	kReal df_;
	kReal low_, high_;
};

