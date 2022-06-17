#include "KcInterpolater.h"
#include "KuInterp1d.h"
#include "KcSampled1d.h"
#include "KtuMath.h"
#include "KuExtrapolate.h"
#include <assert.h>


KcInterpolater::KcInterpolater(std::shared_ptr<KvDiscreted>& cont)
	: internal_(cont)
{
	assert(cont->dim() == 1); // TODO: 暂时只处理一维数据

	interMethod_ = k_linear;
	extraMethod_ = k_nan;
}


kIndex KcInterpolater::dim() const 
{
	return internal_->dim();
}


kIndex KcInterpolater::channels() const 
{
	return internal_->channels();
}


kRange KcInterpolater::range(kIndex axis) const 
{
	return internal_->range(axis);
}


kReal KcInterpolater::value(kReal pt[], kIndex channel) const
{
	assert(channel < internal_->channels());

	if (internal_->size() == 0)
		return extraMethod_ == k_nan ? KtuMath<kReal>::nan : 0;
	else if (internal_->size() == 1) {
		return extraMethod_ == k_nan ? KtuMath<kReal>::nan : 
			extraMethod_ == k_zero ? 0 : internal_->valueAt(kIndex(0), channel);
	}

	kIndex idx = 0;
	auto pt0 = internal_->pointAt(idx, channel);
	kReal xmin = pt0[0];
	idx = internal_->size() - 1;
	auto pt1 = internal_->pointAt(idx, channel);
	kReal xmax = pt1[0];

	auto x = pt[0];
	if (x < xmin || x > xmax) { // 外插
		switch (extraMethod_)
		{
		case k_nan:
			return KtuMath<kReal>::nan;

		case k_zero:
			return 0;
			
		case k_const:
			return x < xmin ? pt0.back() : pt1.back();

		case k_mirro:
			x = KuExtrapolate::mirror(xmin, xmax, x);
			break;

		case k_period:
			x = KuExtrapolate::period(xmin, xmax, x);
			break;

		default:
			// x = x;
			break;
		}
	}

	kIndex numPts = interMethod_ == k_linear ? 2 : 3; // 插值需要的点数目
	numPts = std::min(numPts, internal_->size());

	auto low = internal_->xToLowIndex(x);
	assert(low >= 0 && low < internal_->size());
	while (internal_->size() - low < numPts)
		--low;

	std::vector<std::vector<kReal>> pts(numPts);
	for (kIndex i = 0; i < numPts; i++)
		pts[i] = internal_->pointAt(low++, channel);

	if (numPts == 3)
		return KuInterp1d::quad(pts[0][0], pts[0][1], pts[1][0], pts[1][1], pts[2][0], pts[2][1], x);

	// 线性插值兜底
	return KuInterp1d::linear(pts[0][0], pts[0][1], pts[1][0], pts[1][1], x);
}

