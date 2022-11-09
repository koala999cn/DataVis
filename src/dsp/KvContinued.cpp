#include "KvContinued.h"
#include "KtuMath.h"
#include <assert.h>
#include "KcSampler.h"

kRange KvContinued::valueRange(kIndex channel) const 
{
	if (size() == 0 || dim() > 3) return { 0, 0 };
	for (kIndex i = 0; i < dim(); i++)
		if (length(i) == 0)
			return { 0, 0 };

	std::shared_ptr<KvContinued> cont;
	cont.reset(const_cast<KvContinued*>(this));
	KcSampler samp(cont);

	kReal omin = std::numeric_limits<kReal>::max();
	kReal omax = std::numeric_limits<kReal>::lowest();
	kReal tol(0.001); // 千分之一的误差
	int maxIter(6), numIter(0);

	while (numIter <= maxIter) {
		auto r = samp.valueRange(channel);

		if ((KtuMath<kReal>::almostEqualRel(omin, r.low(), tol) &&
			KtuMath<kReal>::almostEqualRel(omax, r.high(), tol))) {
			if (r.low() < omin) omin = r.low();
			if (r.high() > omax) omax = r.high();
			break;
		}
			
		if (r.low() < omin) omin = r.low();
		if (r.high() > omax) omax = r.high();

		for (kIndex i = 0; i < samp.dim(); i++)
			samp.reset(i, samp.range(i).low(), samp.step(i) / 2);

		if (samp.size() > 1024 * 1024) // TODO: 满足实时性要求
			break;

		++numIter;
	}

	return { omin, omax }; // make compiler happy

/*
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

	return { omin, omax };*/
}
