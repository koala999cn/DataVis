#include "KgResampler.h"
#include "KuInterp1d.h"
#include <assert.h>


KgResampler::KgResampler(int method, kIndex wsize, kIndex chann, double factor)
	: resamp_(wsize, chann, factor)
	, interpMethod_(method)
{

}


KgResampler::interp_t KgResampler::getInterp() const
{
	return [this](const kReal* in, double phase) -> kReal {
		switch (interpMethod_) {
		case k_linear:
			return KuInterp1d::linear(in[0], in[channels()], phase);

		case k_lagrange:
			return KuInterp1d::poly(in, size(), phase, channels());

		default:
			break;
		}
		
		return KuInterp1d::sinc(in, size(), phase, channels());
	};
}


unsigned KgResampler::apply(const kReal* in, kIndex isize, kReal* out, kIndex osize)
{
	return resamp_.apply(in, isize, out, osize, getInterp());
	// TODO: 低通滤波
}


void KgResampler::apply(const kReal* in, kIndex isize, std::vector<kReal>& out)
{
	auto len = resamp_.osize(isize);
	out.resize(len * channels());
	auto olen = apply(in, isize, out.data(), len);
	assert(olen <= len);
	if (olen < len)
		out.resize(olen * channels());
}


std::vector<kReal> KgResampler::apply(const kReal* in, kIndex isize)
{
	std::vector<kReal> out;
	apply(in, isize, out);
	return out;
}


unsigned KgResampler::flush(kReal* out, kIndex osize)
{
	return resamp_.flush(out, osize, getInterp());
}


void KgResampler::flush(std::vector<kReal>& out)
{
	auto fsize = resamp_.fsize();
	out.resize(fsize * channels());
	auto osize = flush(out.data(), fsize);
	assert(osize <= fsize);
	if (osize < fsize)
		out.resize(osize * channels());
}


std::vector<kReal> KgResampler::flush()
{
	std::vector<kReal> out;
	flush(out);
	return out;
}
