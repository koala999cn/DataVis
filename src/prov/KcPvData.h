#pragma once
#include "KvDataProvider.h"

class KvData;

// 基于KvData派生类的数据provider实现

class KcPvData : public KvDataProvider
{
public:

	KcPvData(const std::string_view& name, std::shared_ptr<KvData> data);

	unsigned inPorts() const final { return 0; }

	unsigned outPorts() const final { return 1; }

	bool isStream() const final { return false; }

	kIndex dim() const final;

	kIndex channels() const final;

	kRange range(kIndex axis) const final;

	kReal step(kIndex axis) const final;

	kIndex size(kIndex axis) const final;

	std::shared_ptr<KvData> grabData(kIndex portIdx) override;

	// 对于连续数据，改写range属性为可编辑
	//kPropertySet propertySet() const override;

	//void setOption(KeObjectOption opt, bool on) override;

	//bool getOption(KeObjectOption opt) const override;
	
	//void setPropertyImpl_(int id, const QVariant& newVal) override;

private:
	std::shared_ptr<KvData> data_;
};
