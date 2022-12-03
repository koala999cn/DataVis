#pragma once
#include "KvExcitor.h"


// �������źŷ������Ļ���

class KvOscillator : public KvExcitor
{
public:

	KvOscillator(kReal period) : period_(period) {}

	void reset(kReal phase) { phase_ = phase; }

	kReal freq() const { return freq_; }

	void setFreq(kReal freq) { freq_ = freq; }

	kReal tickRate() const { return tickRate_; }

	void setTickRate(kReal rate) { tickRate_ = rate; }

	kReal tick() override;

	std::pair<kReal, kReal> range() const override {
		return { -1, 1 };
	}

	kReal period() const { return period_; }

private:

	// �˴���phase�ѽ��й�һ����λ��[0, period_)����
	virtual kReal tickImpl_(kReal phase) const = 0;

private:
	kReal period_; // ���źŵı�׼����
	kReal phase_{ 0 };
	kReal freq_{ 440 }; // �ź������Ƶ�ʣ����ڵ�������
	kReal tickRate_{ 44100 }; // �źŷ�����Ƶ�ʣ�������Ƶ��
};

