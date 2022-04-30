#pragma once
#include "KvOpHelper1d.h"
#include <memory>
#include "dsp/KtFIR.h"


// TODO: ��ʹ��KvOpHelper1d
class KcOpFIR : public KvOpHelper1d
{
public:
	KcOpFIR(KvDataProvider* prov);

	kPropertySet propertySet() const override;

	unsigned ins() const final { return 1u; }

	unsigned outs() const final { return 1u; }

private:
	void setPropertyImpl_(int id, const QVariant& newVal) override;

	void processNaive_(const kReal* in, unsigned len, kReal* out) override;

	void syncParent() override;

private:
	std::unique_ptr<KtFIR<kReal>> filter_;
	int type_; // �˲������ͣ���ͨ����ͨ����ͨ�������
	int window_; // ������
	int taps_;
	kReal cutoff0_, cutoff1_;  // ��������Ƶ�ʣ�ȡֵ0 - 0.5
	bool dirty_;
};
