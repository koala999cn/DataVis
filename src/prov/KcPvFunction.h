#pragma once
#include "KcPvData.h"


/// ��ֵ���ʽ

class KcPvFunction : public KcPvData
{
	using super_ = KcPvData;

public:
	KcPvFunction();

	void showPropertySet() override;

	// ����range����
	kRange range(kIndex outPort, kIndex axis) const override;

	void notifyChanged(unsigned outPort = -1) override;

private:

	// �������ݵĲ�������
	void updateDataSamplings_();

private:
	std::vector<std::string> exprs_; // �����ά�ȵ�������ʽ
	std::pair<float, float> iranges_[2]; // ��������
	int counts_[2]; // ��������
	kRange oranges_[2]; // �����������dim0��dim1��ֵ��Χ. NB: dim2��ΪvalueRange����KcPvData����
};
