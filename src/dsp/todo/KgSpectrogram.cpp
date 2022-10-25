#include "KgSpectrogram.h"


KgSpectrogram::KgSpectrogram(const KpOptions& opts)
	: super_(opts, 
		KgSpectrum::KpOptions{
		    opts.frameSize,
		    opts.sampleRate,
		    opts.type,
		    opts.norm,
		    opts.roundToPower2
		})
	, energyFloor_(opts.energyFloor) {

	// �����������ɹ���
	prep_->setHandler([this](double* frame, double energy) {
		std::vector<double> out(odim());
		pipeline_.process(frame, out.data());
		if (prep_->options().useEnergy) {
			out[0] = energy;
			pipeline_.inside<0>().fixPower(out.data(), 1, false); // ��������ֵ
			if (out[0] < energyFloor_)
				out[0] = energyFloor_;
		}

		handler_(out.data());
	});
}
