#include "KgMultiply.h"
#include "KcSampled1d.h"


KgMultiply::KgMultiply(kIndex dim, kIndex channels)
{
	sig_ = std::make_unique<KcSampled1d>();
	sig_->resize(dim, channels);
}


void KgMultiply::process(const KcSampled1d& in, KcSampled1d& out)
{
	assert(in.count() == sig_->count());
}


void KgMultiply::porcess(KcSampled1d& inout)
{
	assert(inout.count() == sig_->count());
}