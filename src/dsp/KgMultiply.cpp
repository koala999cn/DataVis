#include "KgMultiply.h"
#include "KcSampled1d.h"
#include "../base/KtuMath.h"
#include <assert.h>


KgMultiply::KgMultiply(kIndex nx, kIndex channels)
{
	sig_ = std::make_unique<KcSampled1d>();
	sig_->resize(nx, channels);
}


void KgMultiply::process(const KcSampled1d& in, KcSampled1d& out) const
{
	out = in;
	process(out);
}


void KgMultiply::process(KcSampled1d& inout) const
{
	assert(inout.count() == sig_->count());
	process(inout.data(), inout.channels());
}


void KgMultiply::process(kReal* buf, kIndex channels) const
{
	if (channels == sig_->channels()) {
		KtuMath<kReal>::mul(sig_->data(), buf, buf,
			sig_->count() * sig_->channels());
	}
	else {
		assert(sig_->channels() == 1);
		auto sig = sig_->data();
		for (kIndex i = 0; i < sig_->count(); i++, buf += sig_->channels())
			for (kIndex c = 0; c < sig_->channels(); c++)
				buf[c] *= sig[i];
	}
}