#pragma once
#include "KtFeatPipeline.h"
#include "KgSpectrum.h"


class KgSpectrogram : public KtFeatPipeline<KgSpectrum>
{
public:
	using super_ = KtFeatPipeline<KgSpectrum>;

	struct KpOptions : public KgPreprocess::KpOptions
	{
		// ��KgPreprocess::useEnergy��k_use_energy_none, �����ź������滻s0

		KgSpectrum::KeType type;
		KgSpectrum::KeNormMode norm;
		bool roundToPower2;
		double energyFloor; // ��energyС�ڸ�ֵʱ���ø�ֵ����ź�energyֵ����norm�߶����
	};

	KgSpectrogram(const KpOptions& opts);

private:
	double energyFloor_;
};
