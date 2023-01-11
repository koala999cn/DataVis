#include "KcBubble2d.h"
#include "KvPaint.h"
#include "KvDiscreted.h"


unsigned KcBubble2d::majorColorsNeeded() const
{
	return enableColorInterp_ ? super_::majorColorsNeeded() : 1;
}


float KcBubble2d::mapValueToSize_(float_t val) const
{
	if (radiusAsSize_) 
		return KtuMath<float_t>::remap<true>(val, mapLower(), mapUpper(),
			sizeLower_, sizeUpper_);

	auto factor = KtuMath<float_t>::remap<true>(val, mapLower(), mapUpper(),
		sizeLower_ * float_t(sizeLower_), sizeUpper_ * float_t(sizeUpper_));
	return std::sqrt(factor);
}


void KcBubble2d::drawDiscreted_(KvPaint* paint, KvDiscreted* disc) const
{
	assert(disc->channels() == 1);
	assert(disc->dim() <= 2);

	if (!enableColorInterp_)
		paint->setColor(majorColor(0));

	for (unsigned i = 0; i < disc->size(); i++) {
		auto pt = disc->pointAt(i, 0);
		auto val = pt.back();
		if (enableSizeInterp_)
			paint->setMarkerSize(mapValueToSize_(val));
		if (enableColorInterp_)
			paint->setColor(mapValueToColor_(val));

		paint->drawMarker({ pt[0], pt[1], 0 });
	}
}
