#pragma once
#include "KvDataProvider.h"

class KvData;

// 底层基于KvData接口实现的数据快照
class KcDataSnapshot : public KvDataProvider
{
public:
	enum KeDataType
	{
		k_sampled,  // 采样数据
		k_scattered, // 散列数据
		k_continued // 连续数据
	};


	KcDataSnapshot(const QString& name, std::shared_ptr<KvData> data, int type)
		: KvDataProvider(name, false), data_(data), type_(type) {}


	int type() const { return type_; }


	QString typeText() const; 


	bool isStream() const final { return false; }

	unsigned dim() const final;

	kRange range(int axis) const final;

	kReal step(int axis) const final;

	bool pushData() final;

	kPropertySet propertySet() const override;

	std::shared_ptr<KvData> data() { return data_; }


private:
	std::shared_ptr<KvData> data_;
	int type_;
};
