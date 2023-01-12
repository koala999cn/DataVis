#include "KcScatter.h"
#include "plot/KvPaint.h"


void KcScatter::drawImpl_(KvPaint* paint, point_getter getter, unsigned count, unsigned) const
{
	paint->apply(marker_);
	
	bool outline = marker_.showOutline && marker_.hasOutline()
		&& marker_.outline != marker_.fill && marker_.outline.a() > 0;
	paint->drawMarkers(getter, count, outline);
}


unsigned KcScatter::majorColorsNeeded() const
{
	return 1;
}


bool KcScatter::minorColorNeeded() const
{
	return marker_.hasOutline() && marker_.showOutline;
}


unsigned KcScatter::majorColors() const
{
	return 1;
}


color4f KcScatter::majorColor(unsigned idx) const
{
	return marker_.fill;
}


void KcScatter::setMajorColors(const std::vector<color4f>& majors)
{
	assert(majors.size() == 1);
	marker_.fill = majors.front();
}


color4f KcScatter::minorColor() const
{
	return marker_.outline;
}


void KcScatter::setMinorColor(const color4f& minor)
{
	marker_.outline = minor;
}
