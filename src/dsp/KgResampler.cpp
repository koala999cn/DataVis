﻿#include "KgResampler.h"
#include "KuInterp1d.h"
#include <assert.h>


KgResampler::KgResampler(int method, kIndex winlen, kIndex chann, double factor)
	: resamp_(winlen, chann, factor)
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
			return KuInterp1d::poly(in, length(), phase, channels());

		default:
			break;
		}
		
		return KuInterp1d::sinc(in, length(), phase, channels());
	};
}


unsigned KgResampler::apply(const kReal* in, kIndex ilen, kReal* out, kIndex olen)
{
	return resamp_.apply(in, ilen, out, olen, getInterp());
	// TODO: 低通滤波
}


void KgResampler::apply(const kReal* in, kIndex ilen, std::vector<kReal>& out)
{
	auto len = resamp_.olength(ilen);
	out.resize(len * channels());
	auto olen = apply(in, ilen, out.data(), len);
	assert(olen <= len);
	if (olen < len)
		out.resize(olen * channels());
}


std::vector<kReal> KgResampler::apply(const kReal* in, kIndex ilen)
{
	std::vector<kReal> out;
	apply(in, ilen, out);
	return out;
}


unsigned KgResampler::flush(kReal* out, kIndex olen)
{
	return resamp_.flush(out, olen, getInterp());
}


void KgResampler::flush(std::vector<kReal>& out)
{
	auto len = resamp_.flength();
	out.resize(len * channels());
	auto olen = flush(out.data(), len);
	assert(olen <= len);
	if (olen < len)
		out.resize(olen * channels());
}


std::vector<kReal> KgResampler::flush()
{
	std::vector<kReal> out;
	flush(out);
	return out;
}