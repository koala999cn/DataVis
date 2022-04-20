#pragma once
#include "KvDataProvider.h"

class KvData;

// 采样数据provider
class KcPvData : public KvDataProvider
{
public:

	KcPvData(const QString& name, std::shared_ptr<KvData> data);

	bool isStream() const final { return false; }

	kIndex dim() const final;

	kIndex channels() const final;

	kRange range(kIndex axis) const final;

	kReal step(kIndex axis) const final;

	kIndex size(kIndex axis) const final;

	bool running() const override;

	kPropertySet propertySet() const override;

	auto data() { return data_; }

private:
	bool startImpl_() final;
	bool stopImpl_() final;
	
	void setPropertyImpl_(int id, const QVariant& newVal) override {}

private:
	std::shared_ptr<KvData> data_;
};
