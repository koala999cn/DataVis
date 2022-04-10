﻿#pragma once
#include "KvDataProvider.h"

class KvData;

// 底层基于KvData接口实现的数据快照
class KcPvDataSnapshot : public KvDataProvider
{
public:
	enum KeDataType
	{
		k_sampled,  // 采样数据
		k_scattered, // 散列数据
		k_continued // 连续数据
	};


	KcPvDataSnapshot(const QString& name, std::shared_ptr<KvData> data, int type)
		: KvDataProvider(name, false), data_(data), type_(type) {}


	int type() const { return type_; }


	QString typeText() const; 


	bool isStream() const final { return false; }

	kIndex dim() const final;

	kIndex channels() const final;

	kRange range(kIndex axis) const final;

	kReal step(kIndex axis) const final;

	kIndex length(kIndex axis) const final;

	bool pushData() final;

	kPropertySet propertySet() const override;

	void setPropertyImpl_(int id, const QVariant& newVal) override {}

	std::shared_ptr<KvData> data() { return data_; }


private:
	std::shared_ptr<KvData> data_;
	int type_;
};