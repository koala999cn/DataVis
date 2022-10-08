#pragma once
#include "KvDataProvider.h"

class KvData;

// 基于KvData派生类的数据provider实现

class KcPvData : public KvDataProvider
{
public:

	KcPvData(const std::string_view& name, std::shared_ptr<KvData> data);

	bool isStream() const final { return false; }

	kIndex dim() const final;

	kIndex channels() const final;

	kRange range(kIndex axis) const final;

	kReal step(kIndex axis) const final;

	kIndex size(kIndex axis) const final;

	std::shared_ptr<KvData> grabData(kIndex portIdx) override;

private:
	std::shared_ptr<KvData> data_;
};
