#pragma once
#include "KvDataOperator.h"
#include <memory>
#include "dsp/KtFIR.h"


class KcOpFIR : public KvDataOperator
{
	using super_ = KvDataOperator;

public:
	KcOpFIR();

	bool onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins) final;

	void onStopPipeline() final;

	void output() final;

	void showProperySet() final;

	bool permitInput(int dataSpec, unsigned inPort) const final;


private:
	void createFilter_();

private:
	std::unique_ptr<KtFIR<kReal>> filter_;
	int type_; // �˲������ͣ���ͨ����ͨ����ͨ�������
	int window_; // ������
	int taps_;
	float cutoff0_, cutoff1_;  // ��������Ƶ�ʣ�ȡֵ0 - 0.5
	bool dirty_; // filter�����ò����Ƿ����仯
};
