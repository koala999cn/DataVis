#pragma once
#include "KvDataOperator.h"


class KcFramingOp : public KvDataOperator
{
public:
	KcFramingOp(KvDataProvider* prov);

	kPropertySet propertySet() const override;

	void onPropertyChanged(int id, const QVariant& newVal) override;

	bool isStream() const override { return true; }

	kRange range(kIndex axis) const override;

	kReal step(kIndex axis) const override;

	unsigned ins() const final { return 1u; }

	unsigned outs() const final { return 1u; }

private:
	std::shared_ptr<KvData> processImpl_(std::shared_ptr<KvData> data) override;


private:
	kReal length_; // 帧长(s)
	kReal shift_; // 帧移(s)

	std::unique_ptr<KvData> buf_; // 待处理数据，长度小于length_
};

