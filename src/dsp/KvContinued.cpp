#include "KvContinued.h"
#include "KuMath.h"
#include <assert.h>
#include "minmax.h"


kRange KvContinued::valueRange(kIndex channel) const 
{
	if (size() == 0 || dim() > 3) return { 0, 0 };

	int N = std::pow(1024., 1. / dim());
	std::vector<kReal> x0(dim()), x1(dim()), dx(dim());
	for (unsigned i = 0; i < dim(); i++) {
		auto r = range(i);
		x0[i] = r.low();
		x1[i] = r.high();
		KuMath::finiteRange(x0[i], x1[i]);
		dx[i] = (x1[i] - x0[i]) / N;

		if (dx[i] == 0)
			return { 0, 0 };
	}

	return minmax([this, channel](double* x) { return this->value(x, channel); }, 
		dim(), x0.data(), x1.data(), dx.data());
}
