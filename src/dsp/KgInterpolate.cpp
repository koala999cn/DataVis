#include "KgInterpolate.h"
#include "KuInterp1d.h"
#include "KcSampled1d.h"
#include "KtuMath.h"
#include "KuExtrapolate.h"
#include <assert.h>


KgInterpolate::KgInterpolate()
{
	interMethod_ = k_linear;
	extraMethod_ = k_nan;
}


kReal KgInterpolate::process(const KvData& data, kReal x, kIndex channel)
{
	assert(data.dim() == 1); // TODO: 暂时只处理一维数据
	assert(data.isDiscreted());

	const KvDiscreted& dis = (const KvDiscreted&)data;

	if (dis.empty())
		return x;

	kIndex idx = 0;
	auto pt0 = dis.point(idx, channel);
	kReal xmin = pt0[0];
	idx = data.count() - 1;
	auto pt1 = dis.point(idx, channel);
	kReal xmax = pt1[0];

	if (x < xmin || x > xmax) { // 外插
		switch (extraMethod_)
		{
		case k_nan:
			return KtuMath<kReal>::nan;

		case k_zero:
			return 0;
			
		case k_const:
			return x < xmin ? pt0[1] : pt1[1];

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

	const kReal* buf = nullptr;
	std::vector<kReal> temp;
	if (dis.isSampled() && data.channels() == 1)
		buf = ((const KcSampled1d&)data).data();
	else {

	}

	switch (interMethod_)
	{
	case k_linear:
		break;

	case k_quad:
		break;

	case k_hermite:
		break;

	case k_cubic:
		break;

	case k_sinc7:
		break;

	case k_sinc70:
		break;

	case k_sinc700:
		break;

	default:
		break;
	}
}