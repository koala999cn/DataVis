#include "KvContinued.h"
#include "KtuMath.h"
#include <assert.h>
#include "minmax.h"


kRange KvContinued::valueRange(kIndex channel) const 
{
	if (size() == 0 || dim() > 3) return { 0, 0 };
	for (kIndex i = 0; i < dim(); i++)
		if (length(i) == 0)
			return { 0, 0 };

	auto fn = [this, channel](double* x) {
		return this->value(x, channel);
	};

	int N = std::pow(1024., 1. / dim());
	std::vector<kReal> x0(dim()), x1(dim()), dx(dim());
	for (unsigned i = 0; i < dim(); i++) {
		auto r = range(i);
		x0[i] = r.low();
		x1[i] = r.high();
		dx[i] = r.length() / N;
	}

	return minmax(fn, dim(), x0.data(), x1.data(), dx.data());
}
