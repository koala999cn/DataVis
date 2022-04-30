#pragma once
#include "KvDataOperator.h"
#include <memory>
#include "KtFraming.h"


class KcOpFraming : public KvDataOperator
{
public:
	KcOpFraming(KvDataProvider* prov);

	kPropertySet propertySet() const override;

	bool isStream() const override { return true; }

	kIndex dim() const override;

	kRange range(kIndex axis) const override;

	kReal step(kIndex axis) const override;

	kIndex size(kIndex axis) const override;

	unsigned ins() const final { return 1u; }

	unsigned outs() const final { return 2u; }


private:
	void setPropertyImpl_(int id, const QVariant& newVal) override;

	void syncParent() override;

	std::shared_ptr<KvData> processImpl_(std::shared_ptr<KvData> data) override;

	kIndex frameSize() const;
	kIndex shiftSize() const;

private:
	std::unique_ptr<KtFraming<kReal>> framing_;
	kReal dx_;
	kReal frameTime_; // 单帧时长
	kReal shiftTime_; // 帧移时长
};

