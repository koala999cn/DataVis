#pragma once
#include "KvDataOperator.h"
#include <vector>

class KgResampler;

class KcOpResampler : public KvDataOperator
{
public:
	KcOpResampler(KvDataProvider* prov);

	kPropertySet propertySet() const override;

	kReal step(kIndex axis) const override;

	kIndex size(kIndex axis) const override;

	unsigned ins() const final { return 1u; }

	unsigned outs() const final { return 1u; }

private:
	void setPropertyImpl_(int id, const QVariant& newVal) override;

	void syncParent() override;

	std::shared_ptr<KvData> processImpl_(std::shared_ptr<KvData> data) override;

private:
	std::unique_ptr<KgResampler> resamp_;
	kReal factor_; // 重采样系数
	int winlen_;
	int method_;
};

