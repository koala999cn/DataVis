#pragma once
#include "KvDataProvider.h"

class KvData;

// 基于KvData派生类的数据provider实现

class KcPvData : public KvDataProvider
{
	using super_ = KvDataProvider;

public:

	KcPvData(const std::string_view& name, std::shared_ptr<KvData> data);

	int spec(kIndex outPort) const override;

	kRange range(kIndex outPort, kIndex axis) const final;

	kReal step(kIndex outPort, kIndex axis) const final;

	kIndex size(kIndex outPort, kIndex axis) const final;

	std::shared_ptr<KvData> fetchData(kIndex outPort) const final;

	void showProperySet() override;

	std::shared_ptr<KvData> data() const {
		return data_;
	}

	void setData(const std::shared_ptr<KvData>& d);

private:
	void updateSpec_();

private:
	std::shared_ptr<KvData> data_;
	int spec_;
};
