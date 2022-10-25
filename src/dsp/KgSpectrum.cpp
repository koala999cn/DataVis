#include "KgSpectrum.h"
#include "KgRdft.h"
#include "KtuMath.h"
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
	fixPower(buf.data(), odim(), true);
	std::copy(buf.data(), buf.data() + odim(), out);
}


void KgSpectrum::fixPower(double* spec, unsigned c, bool hasNormDefault) const
{
	using kMath = KtuMath<double>;
	constexpr double int16_max = std::numeric_limits<std::int16_t>::max();

	// 谱归一化
	if (opts_.norm == k_norm_praat)
		kMath::scale(spec, c, 1. / (opts_.sampleRate * opts_.sampleRate));
	else if (opts_.norm == k_norm_kaldi)
		kMath::scale(spec, c, int16_max * int16_max);
	else if (opts_.norm == k_norm_default && !hasNormDefault)
		kMath::scale(spec, c, 1. / (double(((KgRdft*)rdft_)->idim()) * ((KgRdft*)rdft_)->idim()));

	// 谱类型转换
	if (opts_.type == k_mag) {
		kMath::forEach(spec, c, [](double x) { return std::sqrt(x); });
	}
	else if (opts_.type == k_log) {
		kMath::applyFloor(spec, c, std::numeric_limits<double>::epsilon());
		kMath::applyLog(spec, c);
	}
	else if (opts_.type == k_db) {
		kMath::applyFloor(spec, c, std::numeric_limits<double>::epsilon());
		kMath::forEach(spec, c, [](double x) { return 10 * std::log10(x); });
	}
}


unsigned KgSpectrum::odim(unsigned frameSize, bool roundToPower2)
{
	if (roundToPower2)
		frameSize = KtuBitwise<unsigned>::ceilPower2(frameSize);
	return frameSize / 2 + 1;
}


const char* KgSpectrum::type2Str(KeType type)
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


const char* KgSpectrum::norm2Str(KeNormMode norm)
{
	switch (norm) {
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