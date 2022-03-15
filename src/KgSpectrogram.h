#pragma once

class KvData;

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

	KgSpectrogram() : type_(k_power), s0UseEnergy_(true) {}

	int type() const { return type_; }
	void setType(int type) { type_ = type; }

	bool useEnergy() const { return s0UseEnergy_; }
	void setUseEnergy(bool b) { s0UseEnergy_ = b; }

private:
	int type_;
	bool s0UseEnergy_; // if true (default), the zeroth spectrogram component is set to the frame energy.
};

