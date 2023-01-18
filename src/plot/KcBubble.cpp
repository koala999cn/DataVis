#include "KcBubble.h"
#include "KvPaint.h"
#include "KvDiscreted.h"


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


void KcBubble::drawImpl_(KvPaint* paint, GETTER getter, unsigned count, unsigned ch) const
{
	paint->apply(marker());
	paint->setMarkerType(KpMarker::k_circle); // ʼ����circle���ͻ�������ͼ

	if (coloringMode() == k_one_color_solid)
		paint->setColor(majorColor(0));

	for (unsigned i = 0; i < count; i++) {
		auto pt = getter(i);
		auto val = pt[data()->dim()]; // TODO: �ߴ��ֵ������ά�ȿ�����
		paint->setMarkerSize(mapValueToSize_(val));
		if (coloringMode() != k_one_color_solid)
			paint->setColor(mapValueToColor_(pt.data(), ch));
		
		paint->drawMarker(toPoint_(pt.data(), ch));
	}
}
