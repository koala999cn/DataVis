#include "KcBars2d.h"
#include "KvPaint.h"
#include "KvDiscreted.h"
#include "KvContinued.h"


KcBars2d::KcBars2d(const std::string_view& name)
	: super_(name)
{
	border_.color = color4f(0, 0, 0, 1); // default no border 
}


void KcBars2d::drawImpl_(KvPaint* paint, point_getter getter, unsigned count, unsigned) const
{
	auto barWidth = barWidth_();
	bool drawFill = fill_.style != KpBrush::k_none && majorColor(0).a() != 0;
	bool drawBorder = border_.style != KpPen::k_none && minorColor().a() != 0 && minorColor() != majorColor(0);

	for (unsigned i = 0; i < count; i++) {
		auto pt0 = getter(i);
		pt0.x() -= barWidth * 0.5;
		point3 pt1(pt0.x() + barWidth, baseLine_, pt0.z());

		if (drawFill) {
			paint->apply(fill_);
			paint->fillRect(pt0, pt1);
		}

		if (drawBorder) {
			paint->apply(border_);
			paint->drawRect(pt0, pt1);
		}
	}
}


KcBars2d::float_t KcBars2d::barWidth_(unsigned dim) const
{
	if (data()->isDiscreted()) {
		auto disc = std::dynamic_pointer_cast<KvDiscreted>(data());
		assert(disc->size(dim) != 0);

		return disc->step(dim) != 0 ?
			disc->step(dim) * barWidthRatio_ :
			disc->range(dim).length() / disc->size(dim) * barWidthRatio_;
	}
	else {
		assert(sampCount(dim) != 0);
		auto cont = std::dynamic_pointer_cast<KvContinued>(data());
		return cont->range(dim).length() / sampCount(dim) * barWidthRatio_;
	}
}


KcBars2d::aabb_t KcBars2d::boundingBox() const
{
	auto aabb = super_::boundingBox();
	if (aabb.lower().y() > baseLine_)
		aabb.lower().y() = baseLine_;
	if (aabb.upper().y() < baseLine_)
		aabb.upper().y() = baseLine_;

	if (data()) {
		auto w = barWidth_();
		aabb.lower().x() -= w;
		aabb.upper().x() += w;
	}

	return aabb;
}


unsigned KcBars2d::majorColorsNeeded() const
{
	return 1;
}


bool KcBars2d::minorColorNeeded() const
{
	return true;
}


unsigned KcBars2d::majorColors() const
{
	return 1;
}


color4f KcBars2d::majorColor(unsigned idx) const
{
	return fill_.color;
}


void KcBars2d::setMajorColors(const std::vector<color4f>& majors)
{
	assert(majors.size() == 1);
	fill_.color = majors.front();
}


color4f KcBars2d::minorColor() const
{
	return border_.color;
}


void KcBars2d::setMinorColor(const color4f& minor)
{
	border_.color = minor;
}
