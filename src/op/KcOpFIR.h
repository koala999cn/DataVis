#pragma once
#include "KvDataOperator.h"
#include <memory>
#include "dsp/KtFIR.h"


// TODO: 不使用KvOpHelper1d
class KcOpFIR : public KvDataOperator
{
public:
	KcOpFIR(KvDataProvider* prov);

	kPropertySet propertySet() const override;

	unsigned ins() const final { return 1u; }

	unsigned outs() const final { return 1u; }

private:
	void setPropertyImpl_(int id, const QVariant& newVal) override;

	std::shared_ptr<KvData> processImpl_(std::shared_ptr<KvData> data) override;

	void syncParent() override;

private:
	std::unique_ptr<KtFIR<kReal>> filter_;
	int type_; // 滤波器类型：低通、高通、带通、带阻等
	int window_; // 窗类型
	int taps_;
	kReal cutoff0_, cutoff1_;  // 两个截至频率，取值0 - 0.5
	bool dirty_;
};
