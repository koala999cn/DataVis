#pragma once

class KcSampled1d;
class KcSampled2d;


class KgSpectrogram
{
public:

	enum KeType
	{
		k_power, // |FFT|^2
		k_log, // log(|FFT|^2)
		k_db, // 10*log10(|FFT|^2)
		k_mag // |FFT|
	};

	KgSpectrogram() : type_(k_power), useEnergy_(true) {}

	int type() const { return type_; }
	void setType(int type) { type_ = type; }

	bool useEnergy() const { return useEnergy_; }
	void setUseEnergy(bool b) { useEnergy_ = b; }

	void process(const KcSampled1d& in, KcSampled2d& out);

private:
	int type_;

	// if true (default), the zeroth spectrogram component is set to the frame energy.
	bool useEnergy_;
};

