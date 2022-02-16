#pragma once
#include "KvInputSource.h"

class KvData;

// 数据快照输入源
class KcDataSnapshot : public KvInputSource
{
public:
	enum KeDataType
	{
		k_sampled,  // 采样数据
		k_scattered, // 散列数据
		k_continued // 连续数据
	};


	KcDataSnapshot(const QString& name, std::shared_ptr<KvData> data, int type)
		: KvInputSource(name, false), data_(data), type_(type) {}


	int type() const { return type_; }


	QString typeText() const; 

	kPropertySet propertySet() const override;

	std::shared_ptr<KvData> data() { return data_; }

private:
	std::shared_ptr<KvData> data_;
	int type_;
};
