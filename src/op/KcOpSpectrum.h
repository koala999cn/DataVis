#pragma once
#include "KvDataOperator.h"


class KgSpectrum;

class KcOpSpectrum : public KvDataOperator
{
public:

	KcOpSpectrum(KvDataProvider* prov);

	kPropertySet propertySet() const override;

	bool isStream() const override;

	kRange range(kIndex axis) const override;

	kReal step(kIndex axis) const override;

	kIndex size(kIndex axis) const override;

	unsigned ins() const final { return 1u; }

	unsigned outs() const final { return 1u; }


private:
	void setPropertyImpl_(int id, const QVariant& newVal) override;
	void preRender_() override;

	std::shared_ptr<KvData> processImpl_(std::shared_ptr<KvData> data) override;

	std::shared_ptr<KvData> process1d_(std::shared_ptr<KvData> data);
	std::shared_ptr<KvData> process2d_(std::shared_ptr<KvData> data);

private:
	std::unique_ptr<KgSpectrum> spec_;
};

