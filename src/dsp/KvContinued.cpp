#include "KvContinued.h"
#include "KtuMath.h"
#include <assert.h>


kRange KvContinued::valueRange(kIndex channel) const 
{
	if (size() == 0) return { 0, 0 };

	assert(dim() == 1); // TODO: 暂时实现一维算法

	kReal omin = std::numeric_limits<kReal>::max();
	kReal omax = std::numeric_limits<kReal>::lowest();

	kReal low = range(0).low();
	if (std::isinf(low)) low = -1e8;
	kReal high = range(0).high();
	if (std::isinf(high)) high = 1e8;
	kReal dx((high - low) / 2);
	kReal tol(0.001); // 百分之一的误差
	int minIter(10), maxIter(16), numIter(0);

	while (true) {
		kReal nmin = std::numeric_limits<kReal>::max();
		kReal nmax = std::numeric_limits<kReal>::lowest();
		for (kReal x = low; x < high; x += dx) {
			auto val = value(&x, channel);
			if (val > nmax)
				nmax = val;
			if (val < nmin)
				nmin = val;
		}

		if ((KtuMath<kReal>::almostEqualRel(omin, nmin, tol) &&
			KtuMath<kReal>::almostEqualRel(omax, nmax, tol) &&
			numIter > minIter) || numIter > maxIter)
			return { nmin, nmax };


		omin = nmin, omax = nmax, dx *= 0.5;
		++numIter;
	}

	return { omin, omax };
}
