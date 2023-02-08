#pragma once
#include "KcPvData.h"


/// ��ֵ���ʽ

class KcPvFunction : public KcPvData
{
	using super_ = KcPvData;

public:
	KcPvFunction();

	void showPropertySet() override;

private:

	// �������ݵĲ�������
	void updateDataSamplings_();

private:
	std::vector<std::string> exprs_; // �����ά�ȵ�������ʽ
	std::pair<float, float> ranges_[2];
	int counts_[2];
};
