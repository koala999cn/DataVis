#include "KcScatter.h"
#include "plot/KvPaint.h"


void KcScatter::drawImpl_(KvPaint* paint, GETTER getter, unsigned count, unsigned ch) const
{
	auto clr = majorColor(ch);

	if (showLine_ && lineCxt_.visible()) {
		paint->apply(lineCxt_);
		paint->setColor(clr);
		paint->drawLineStrip(toPointGetter_(getter, ch), count);
	}

	marker_.fill = clr;
	paint->apply(marker_);
	
	bool outline = marker_.showOutline && marker_.hasOutline()
		&& marker_.outline != marker_.fill && marker_.outline.a() > 0;

	if (coloringMode() == k_one_color_solid) {
		paint->drawMarkers(toPointGetter_(getter, ch), count, outline);
	}
	else { // Öð¸ömarker»æÖÆ
		for (unsigned i = 0; i < count; i++) {
			auto val = getter(i);
			paint->setColor(mapValueToColor_(val.data(), ch));
			paint->drawMarker({ val[0], val[1], val[2] }, outline);
		}
	}
}


const color4f& KcScatter::minorColor() const
{
	return marker_.outline;
}


void KcScatter::setMinorColor_(const color4f& minor)
{
	marker_.outline = minor;
}
