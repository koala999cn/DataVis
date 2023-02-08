#pragma once
#include "KvDataProvider.h"

class KvData;

// 基于KvData派生类的数据provider实现（单输出端口）

class KcPvData : public KvDataProvider
{
	using super_ = KvDataProvider;

public:

	KcPvData(const std::string_view& name, std::shared_ptr<KvData> data);

	bool onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>&) override;

	int spec(kIndex outPort) const override;

	kRange range(kIndex outPort, kIndex axis) const final;

	kReal step(kIndex outPort, kIndex axis) const final;

	kIndex size(kIndex outPort, kIndex axis) const final;

	std::shared_ptr<KvData> fetchData(kIndex outPort) const final;

	unsigned dataStamp(kIndex outPort) const override;

	void showPropertySet() override;

	std::shared_ptr<KvData> data() const {
		return data_;
	}

	void setData(const std::shared_ptr<KvData>& d);

	// 当data_在外部发生了更改时，须显示调用该函数
	virtual void notifyChanged(bool specChanged, bool dataChanged);

private:
	void updateSpec_();

private:
	std::shared_ptr<KvData> data_;
	int spec_;
	kRange valueRange_; // 缓存数据的值域，避免重复计算耗费资源
	unsigned dataStamp_{ 0 };
};
