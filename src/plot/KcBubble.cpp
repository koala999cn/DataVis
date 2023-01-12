#include "KcBubble.h"
#include "KvPaint.h"
#include "KvDiscreted.h"


unsigned KcBubble::majorColorsNeeded() const
{
	return enableColorInterp_ ? super_::majorColorsNeeded() : 1;
}


float KcBubble::mapValueToSize_(float_t val) const
{
	auto vrange = data()->valueRange();

	if (radiusAsSize_) 
		return KtuMath<float_t>::remap<true>(val, vrange.low(), vrange.high(),
			sizeLower_, sizeUpper_);

	auto factor = KtuMath<float_t>::remap<true>(val, vrange.low(), vrange.high(),
		sizeLower_ * float_t(sizeLower_), sizeUpper_ * float_t(sizeUpper_));

	return std::sqrt(factor);
}


void KcBubble::drawImpl_(KvPaint* paint, point_getter1 getter, unsigned count, unsigned chs) const
{
	paint->setMarkerType(KpMarker::k_circle);

	if (!enableColorInterp_)
		paint->setColor(majorColor(0));

	for (unsigned i = 0; i < count; i++) {
		auto pt = getter(i);
		auto val = pt[data()->dim()];
		if (enableSizeInterp_)
			paint->setMarkerSize(mapValueToSize_(val));
		//if (enableColorInterp_)
		//	paint->setColor(mapValueToColor_(val));
		
		paint->drawMarker({ pt[0], pt[1], pt[2] });
	}
}
