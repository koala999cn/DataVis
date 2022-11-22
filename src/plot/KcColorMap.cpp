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

	if (!empty() && data()->dim() > 1) {

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
	auto dx = disc->step(0);
	auto dy = disc->dim() > 1 ? disc->step(1) : 0;
	if (dx <= 0)
		dx = disc->range(0).length() / disc->size(0);
	if (dy <= 0)
		dy = disc->range(1).length() / disc->size(disc->dim() > 1 ? 1 : 0);

	auto half_dx = dx / 2;
	auto hfalf_dy = dy /2 ;

	for (unsigned i = 0; i < disc->size(); i++) {
		auto pt = disc->pointAt(i, 0);
		paint->setColor(mapValueToColor_(pt.back()));
		paint->fillRect({ pt[0] - half_dx, pt[1] - hfalf_dy, 0 },
			{ pt[0] + half_dx, pt[1] + hfalf_dy, 0 });
	}

	paint->setColor(color4f(1, 0, 0, 1)); // TODO:
	auto leng = paint->projectv({ dx, -dy, 0 });
	for (unsigned i = 0; i < disc->size(); i++) {
		auto pt = disc->pointAt(i, 0);
		auto text = KuStrUtil::toString(pt.back());
		auto szText = paint->textSize(text.c_str());
		if (szText.x() <= leng.x() && szText.y() <= leng.y())
		    paint->drawText({ pt[0], pt[1], 0 }, text.c_str(), 0);
	}
}


color4f KcColorMap::mapValueToColor_(float_t val) const
{
	auto factor = KtuMath<float_t>::remap<true>(val, valLower_, valUpper_);
	return mapper_.getAt(factor);
}

