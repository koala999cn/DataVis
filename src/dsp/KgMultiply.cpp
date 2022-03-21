#include "KgMultiply.h"
#include "KcSampled1d.h"
#include "../base/KtuMath.h"


KgMultiply::KgMultiply(kIndex dim, kIndex channels)
{
	sig_ = std::make_unique<KcSampled1d>();
	sig_->resize(dim, channels);
}


void KgMultiply::process(const KcSampled1d& in, KcSampled1d& out)
{
	assert(in.count() == sig_->count());
	assert(in.channels() == sig_->channels());
	out.resize(in.count(), in.channels());
	KtuMath<kReal>::mul(sig_->data(), in.data(), const_cast<kReal*>(out.data()), 
		in.count() * in.channels());
}


void KgMultiply::porcess(KcSampled1d& inout)
{
	assert(inout.count() == sig_->count());
	assert(inout.channels() == sig_->channels());
	KtuMath<kReal>::mul(sig_->data(), inout.data(), const_cast<kReal*>(inout.data()), 
		inout.count() * inout.channels());
}


