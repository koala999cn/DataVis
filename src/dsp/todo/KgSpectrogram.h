#pragma once
#include "KtFeatPipeline.h"
#include "KgSpectrum.h"


class KgSpectrogram : public KtFeatPipeline<KgSpectrum>
{
public:
	using super_ = KtFeatPipeline<KgSpectrum>;

	struct KpOptions : public KgPreprocess::KpOptions
	{
		// 若KgPreprocess::useEnergy非k_use_energy_none, 则用信号能量替换s0

		KgSpectrum::KeType type;
		KgSpectrum::KeNormMode norm;
		bool roundToPower2;
		double energyFloor; // 当energy小于该值时，用该值替代信号energy值，与norm高度相关
	};

	KgSpectrogram(const KpOptions& opts);

private:
	double energyFloor_;
};
