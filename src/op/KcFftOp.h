#pragma once
#include "KvDataOperator.h"


class KgRdft;

class KcFftOp : public KvDataOperator
{
public:

	KcFftOp(KvDataProvider* prov);

	kPropertySet propertySet() const override;

	bool isStream() const override;

	kRange range(kIndex axis) const override;

	kReal step(kIndex axis) const override;

	kIndex length(kIndex axis) const override;

	unsigned ins() const final { return 1u; }

	unsigned outs() const final { return 1u; }


private:
	void setPropertyImpl_(int id, const QVariant& newVal) override;
	void syncParent() override;

	std::shared_ptr<KvData> processImpl_(std::shared_ptr<KvData> data) override;

	std::shared_ptr<KvData> process1d_(std::shared_ptr<KvData> data);
	std::shared_ptr<KvData> process2d_(std::shared_ptr<KvData> data);

	void postProcess_(kReal* data) const; // 根据type_对data进行变换

private:
	std::unique_ptr<KgRdft> rdft_;
	kReal df_;
	kReal nyquistFreq_;
	int type_; // 频谱类型
	kReal floor_; // 频谱底值，适用于log谱
};

