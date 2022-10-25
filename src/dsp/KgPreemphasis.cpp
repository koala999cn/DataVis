#include "KgPreemphasis.h"
#include <assert.h>


KgPreemphasis::KgPreemphasis(unsigned frameSize, double coeff) 
	: frameSize_(frameSize), preemphasis_(coeff) 
{
	assert(preemphasis_ >= 0.0 && preemphasis_ <= 1.0);
}


void KgPreemphasis::porcess(double* x) const
{
	if (preemphasis_ == 0.0) return;
	
	for (long i = idim() - 1; i > 0; i--)
		x[i] -= preemphasis_ * x[i - 1];

	x[0] -= preemphasis_ * x[0]; // ²Î¿¼kladiÌí¼Ó
}

