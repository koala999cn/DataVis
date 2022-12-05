#include "KvOscillator.h"
#include "KuExtrapolate.h"
#include <cmath>
#include <assert.h>


kReal KvOscillator::tick() 
{
	phase_ += period_ * freq_ / tickRate_; // �˴����Ż�
	phase_ = fmod(phase_, period_);
	assert(phase_ >= 0 && phase_ < period_);
	return tickImpl_(phase_);
}
