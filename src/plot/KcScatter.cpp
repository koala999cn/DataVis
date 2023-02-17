#include "KcScatter.h"
#include "plot/KvPaint.h"


bool KcScatter::objectVisible_(unsigned objIdx) const
{
	if (objIdx & 1)
		return showLine_ && lineCxt_.visible();
	else
		return true;
}


void KcScatter::setObjectState_(KvPaint* paint, unsigned objIdx) const
{
	if (objIdx & 1) { // line
		paint->apply(lineCxt_);
	}
	else { // marker
		paint->apply(marker_);
		paint->setEdged(marker_.showOutline && marker_.hasOutline()
			&& marker_.outline != marker_.fill && marker_.outline.a() > 0);
	}

	paint->setColor(majorColor(objIdx / 2));
}


void* KcScatter::drawObjectImpl_(KvPaint* paint, GETTER getter, unsigned count, unsigned objIdx) const
{
	auto ch = objIdx / 2;

	if (objIdx & 1) 
		return paint->drawLineStrip(toPoint3Getter_(getter, ch), count);

	if (coloringMode() == k_one_color_solid) {
		return paint->drawMarkers(toPoint3Getter_(getter, ch), count);
	}
	else { // Öð¸ömarker»æÖÆ
		for (unsigned i = 0; i < count; i++) {
			auto val = getter(i);
			paint->setColor(mapValueToColor_(val.data(), ch));
			paint->drawMarker({ val[0], val[1], val[2] });
		}
	}

	return nullptr;
}


const color4f& KcScatter::minorColor() const
{
	return marker_.outline;
}


void KcScatter::setMinorColor_(const color4f& minor)
{
	marker_.outline = minor;
}
