#include "KgHistC.h"
#include <assert.h>


void KgHistC::reset()
{
	std::fill(res_.begin(), res_.end(), 0);
}


void KgHistC::count(const kReal* data, kIndex N)
{
	assert(res_.size() == numBins());

	for (kIndex i = 0; i < N; i++) {
		auto idx = binIndex(data[i]);
		if (idx != -1)
		    res_[idx]++;
	}
}

