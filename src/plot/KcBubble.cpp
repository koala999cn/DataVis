#include "KcBubble.h"
#include "KvPaint.h"
#include "KvDiscreted.h"
#include "KuStrUtil.h"


float KcBubble::mapValueToSize_(float_t val) const
{
	auto vrange = data()->valueRange();

	if (radiusAsSize_) 
		return KuMath::remap<float_t, true>(val, vrange.low(), vrange.high(),
			sizeLower_, sizeUpper_);

	auto factor = KuMath::remap<float_t, true>(val, vrange.low(), vrange.high(),
		sizeLower_ * float_t(sizeLower_), sizeUpper_ * float_t(sizeUpper_));

	return std::sqrt(factor);
}


void KcBubble::drawImpl_(KvPaint* paint, GETTER getter, unsigned count, unsigned ch) const
{
	paint->setMarkerType(KpMarker::k_circle);

	bool realShowText = showText_ && clrText_.a() != 0;

	for (unsigned i = 0; i < count; i++) {
		auto pt = getter(i);
		auto val = pt[data()->dim()]; // TODO: 尺寸插值的数据维度可配置
		paint->setMarkerSize(mapValueToSize_(val));
		paint->setColor(mapValueToColor_(pt.data(), ch));
		paint->drawMarker(toPoint_(pt.data(), ch), false);

		if (realShowText) {
			auto text = KuStrUtil::toString(pt[colorMappingDim()]);
			auto szText = paint->textSize(text.c_str());
			paint->setColor(textColor());
			paint->drawText(toPoint_(pt.data(), ch), text.c_str(),
				KeAlignment::k_vcenter | KeAlignment::k_hcenter);
		}
	}
}


const color4f& KcBubble::minorColor() const
{
	return clrMinor_;
}


void KcBubble::setMinorColor_(const color4f& minor)
{
	clrMinor_ = minor;
}
