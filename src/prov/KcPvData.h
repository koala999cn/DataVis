#pragma once
#include "KvDataProvider.h"

class KvData;

// 基于KvData派生类的数据provider实现

class KcPvData : public KvDataProvider
{
public:

	KcPvData(const std::string_view& name, std::shared_ptr<KvData> data);

	bool isStream(kIndex outPort) const final { return false; }

	kIndex dim(kIndex outPort) const final;

	kIndex channels(kIndex outPort) const final;

	kRange range(kIndex outPort, kIndex axis) const final;

	kReal step(kIndex outPort, kIndex axis) const final;

	kIndex size(kIndex outPort, kIndex axis) const final;

	std::shared_ptr<KvData> fetchData(kIndex outPort) const final;

protected:
	std::shared_ptr<KvData> data_;
};
