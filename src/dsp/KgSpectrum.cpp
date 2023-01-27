#include "KgSpectrum.h"
#include "KgRdft.h"
#include "KuMath.h"
#include "base/KtuBitwise.h"


KgSpectrum::KgSpectrum(const KpOptions& opts)
	: opts_(opts)
{
	auto fftsize = opts.frameSize;
	if (opts.roundToPower2)
		fftsize = KtuBitwise<unsigned>::ceilPower2(fftsize);

	rdft_ = new KgRdft(fftsize, false, k_norm_default == opts.norm);
}


KgSpectrum::KgSpectrum(KgSpectrum&& spec) noexcept
	: opts_(spec.opts_)
{
	std::swap(rdft_, spec.rdft_);
}


KgSpectrum::~KgSpectrum()
{
	delete (KgRdft*)rdft_;
}


unsigned KgSpectrum::idim() const
{
	return opts_.frameSize;
}


unsigned KgSpectrum::odim() const
{
	return ((KgRdft*)rdft_)->odim();
}


void KgSpectrum::process(const double* in, double* out) const
{
	auto rdft = ((KgRdft*)rdft_);

	// 此处传入的in是原始尺寸，根据需要roundToPower2，以作为fft的输入
	std::vector<double> buf(in, in + idim());
	if (idim() != rdft->idim()) { // roundToPower2
		buf.resize(rdft->idim());
		std::fill(buf.begin() + idim(), buf.end(), 0);
	}

	rdft->forward(buf.data());
	rdft->powerSpectrum(buf.data()); // 功率谱
	fixPower(buf.data(), odim(), opts_.norm == k_norm_default);
	std::copy(buf.data(), buf.data() + odim(), out);
}


void KgSpectrum::fixPower(double* spec, unsigned c, bool hasNormDefault) const
{
	constexpr double int16_max = std::numeric_limits<std::int16_t>::max();

	// 谱归一化
	if (opts_.norm == k_norm_praat)
		KuMath::scale(spec, c, 1. / (opts_.sampleRate * opts_.sampleRate));
	else if (opts_.norm == k_norm_kaldi)
		KuMath::scale(spec, c, int16_max * int16_max);
	else if (opts_.norm == k_norm_default && !hasNormDefault)
		KuMath::scale(spec, c, 1. / (double(((KgRdft*)rdft_)->idim()) * ((KgRdft*)rdft_)->idim()));

	// 谱类型转换
	if (opts_.type == k_mag) {
		KuMath::forEach(spec, c, [](double x) { return std::sqrt(x); });
	}
	else if (opts_.type == k_log) {
		KuMath::applyFloor(spec, c, std::numeric_limits<double>::epsilon());
		KuMath::applyLog(spec, c);
	}
	else if (opts_.type == k_db) {
		KuMath::applyFloor(spec, c, std::numeric_limits<double>::epsilon());
		KuMath::forEach(spec, c, [](double x) { return 10 * std::log10(x); });
	}
}


std::pair<double, double> KgSpectrum::orange(const std::pair<double, double>& in) const
{
	// 假设原始信号的峰值为A，那么FFT的结果的每个点（除了第一个点直流分量之外）的模值就是A
	// 的N / 2倍。而第一个点就是直流分量，它的模值就是直流分量的N倍。

	auto N = idim();
	double mag = KuMath::absMax(in.first, in.second);
	mag *= N;
	mag *= mag;
	fixPower(&mag, 1, false);
	return { 0, mag };
}


unsigned KgSpectrum::odim(unsigned idim, bool roundToPower2)
{
	if (roundToPower2)
		idim = KtuBitwise<unsigned>::ceilPower2(idim);
	return idim / 2 + 1;
}


const char* KgSpectrum::type2Str(int type)
{
	switch (type) {
	case k_power:	return "power";
	case k_log:		return "log";
	case k_db:		return "db";
	case k_mag:		return "mag";
	default:		return "unknown";
	}
}


KgSpectrum::KeType KgSpectrum::str2Type(const char* str)
{
	if (0 == _stricmp(str, type2Str(k_power)))
		return k_power;

	if (0 == _stricmp(str, type2Str(k_log)))
		return k_log;

	if (0 == _stricmp(str, type2Str(k_db)))
		return k_db;

	if (0 == _stricmp(str, type2Str(k_mag)))
		return k_mag;

	return k_power;
}


const char* KgSpectrum::norm2Str(int normMode)
{
	switch (normMode) {
	case k_norm_none:		return "none";
	case k_norm_default:	return "default";
	case k_norm_praat:		return "praat";
	case k_norm_kaldi:		return "kaldi";
	default:				return "unknown";
	}
}


KgSpectrum::KeNormMode KgSpectrum::str2Norm(const char* str)
{
	if (0 == _stricmp(str, norm2Str(k_norm_none)))
		return k_norm_none;

	if (0 == _stricmp(str, norm2Str(k_norm_default)))
		return k_norm_default;

	if (0 == _stricmp(str, norm2Str(k_norm_praat)))
		return k_norm_praat;

	if (0 == _stricmp(str, norm2Str(k_norm_kaldi)))
		return k_norm_kaldi;

	return k_norm_none;
}