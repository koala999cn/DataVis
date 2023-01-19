#include "KcScatter.h"
#include "plot/KvPaint.h"


void KcScatter::drawImpl_(KvPaint* paint, GETTER getter, unsigned count, unsigned ch) const
{
	marker_.fill = majorColor(ch);
	paint->apply(marker_);
	
	bool outline = marker_.showOutline && marker_.hasOutline()
		&& marker_.outline != marker_.fill && marker_.outline.a() > 0;
	paint->drawMarkers(toPointGetter_(getter, ch), count, outline);
}


const color4f& KcScatter::minorColor() const
{
	return marker_.outline;
}


void KcScatter::setMinorColor(const color4f& minor)
{
	marker_.outline = minor;
}
