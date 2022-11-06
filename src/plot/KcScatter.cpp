#include "KcScatter.h"
#include "plot/KvPaint.h"


void KcScatter::drawImpl_(KvPaint* paint, point_getter getter, unsigned count, unsigned) const
{
	paint->apply(scatBrush_);
	paint->setPointSize(size_);
	paint->drawPoints(getter, count);
}


unsigned KcScatter::majorColorsNeeded() const
{
	return 1;
}


bool KcScatter::minorColorNeeded() const
{
	return true;
}


unsigned KcScatter::majorColors() const
{
	return 1;
}


const color4f& KcScatter::majorColor(unsigned idx) const
{
	return scatBrush_.color;
}


color4f& KcScatter::majorColor(unsigned idx)
{
	return scatBrush_.color;
}


void KcScatter::setMajorColors(const std::vector<color4f>& majors)
{
	assert(majors.size() == 1);
	scatBrush_.color = majors.front();
}


const color4f& KcScatter::minorColor() const
{
	return scatPen_.color;
}


color4f& KcScatter::minorColor()
{
	return scatPen_.color;
}


void KcScatter::setMinorColor(const color4f& minor)
{
	scatPen_.color = minor;
}
