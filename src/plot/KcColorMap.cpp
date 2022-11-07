#include "KcColorMap.h"
#include "KvPaint.h"
#include "KvData.h"
#include "KcSampled2d.h"
#include "KtuMath.h"
#include "KuStrUtil.h"


KcColorMap::KcColorMap(const std::string_view& name)
	: super_(name)
{
	mapper_.setAt(0, color4f(0, 0, 0, 1));
	mapper_.setAt(1, color4f(1, 1, 1, 1));
}


unsigned KcColorMap::majorColorsNeeded() const
{
	return -1;
}


bool KcColorMap::minorColorNeeded() const
{
	return 1;
}


unsigned KcColorMap::majorColors() const
{
	return mapper_.numStops();
}


color4f KcColorMap::majorColor(unsigned idx) const
{
	return mapper_.stopAt(idx).second;
}


void KcColorMap::setMajorColors(const std::vector<color4f>& majors)
{
	mapper_.reset();

	std::vector<float_t> vals(majors.size());
	KtuMath<float_t>::linspace(0, 1, 0, vals.data(), majors.size());
	for (unsigned i = 0; i < majors.size(); i++)
		mapper_.setAt(vals[i], majors[i]);
}


color4f KcColorMap::minorColor() const
{
	return border_;
}


void KcColorMap::setMinorColor(const color4f& minor)
{
	border_ = minor;
}


KcColorMap::aabb_type KcColorMap::boundingBox() const
{
	auto aabb = super_::boundingBox();

	if (!empty()) {

		float_t dx, dy;

		if (data()->isDiscreted()) {
			auto disc = std::dynamic_pointer_cast<KvDiscreted>(data());
			assert(disc->size() != 0);
			dx = disc->step(0);
			dy = disc->step(1);
		}
		else {
			dx = sampCount(0) == 0 ? 0 : data()->range(0).length() / sampCount(0);
			dy = sampCount(1) == 0 ? 0 : data()->range(1).length() / sampCount(1);
		}

		dx *= 0.5; dy *= 0.5;

		aabb.lower().x() -= dx;
		aabb.lower().y() -= dy;
		aabb.upper().x() += dx;
		aabb.upper().y() += dy;
	}

	return aabb;
}


void KcColorMap::drawDiscreted_(KvPaint* paint, KvDiscreted* disc) const
{
	assert(disc->dim() == 2 && disc->isSampled());

	auto samp = dynamic_cast<KvSampled*>(disc);
	assert(samp);

	auto half_dx = samp->step(0) / 2;
	auto hfalf_dy = samp->step(1) /2 ;

	for (unsigned i = 0; i < samp->size(); i++) {
		auto pt = samp->pointAt(i, 0);
		paint->setColor(mapValueToColor_(pt.back()));
		paint->fillRect({ pt[0] - half_dx, pt[1] - hfalf_dy, 0 },
			{ pt[0] + half_dx, pt[1] + hfalf_dy, 0 });
	}

	paint->setColor(color4f(1, 0, 0, 1)); // TODO:
	for (unsigned i = 0; i < samp->size(); i++) {
		auto pt = samp->pointAt(i, 0);
		auto text = KuStrUtil::toString(pt.back());
		auto szText = paint->textSize(text.c_str());
		if (szText.x() <= samp->step(0) &&
			szText.y() <= samp->step(1))
		    paint->drawText({ pt[0], pt[1], 0 }, text.c_str(), 0);
	}
}


color4f KcColorMap::mapValueToColor_(float_t val) const
{
	auto factor = val - valLower_;
	auto delta = valUpper_ - valLower_;
	if (delta == 0)
		factor = factor > 0 ? 1 : 0;
	else
		factor /= delta;

	return mapper_.getAt(factor);
}

