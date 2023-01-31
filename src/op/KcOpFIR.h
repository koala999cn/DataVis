#pragma once
#include "KvOp1to1.h"
#include <memory>
#include "dsp/KtFIR.h"


class KcOpFIR : public KvOp1to1
{
	using super_ = KvOp1to1;

public:
	KcOpFIR();

	bool onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins) final;

	void onStopPipeline() final;

	void showProperySet() final;

	bool permitInput(int dataSpec, unsigned inPort) const final;


private:
	void outputImpl_() final;

	void createFilter_();

private:
	std::unique_ptr<KtFIR<kReal>> filter_;
	int type_; // �˲������ͣ���ͨ����ͨ����ͨ�������
	int window_; // ������
	int taps_;
	float cutoff0_, cutoff1_;  // ��������Ƶ�ʣ�ȡֵ0 - 0.5
	bool dirty_; // filter�����ò����Ƿ����仯
};
