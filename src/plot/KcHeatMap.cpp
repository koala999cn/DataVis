#include "KcHeatMap.h"
#include "KvDiscreted.h"
#include "KuStrUtil.h"
#include "KvPaint.h"


KcHeatMap::aabb_t KcHeatMap::boundingBox() const
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


void KcHeatMap::drawImpl_(KvPaint* paint, point_getter2 getter, unsigned nx, unsigned ny, unsigned ch) const
{
	auto disc = std::dynamic_pointer_cast<KvDiscreted>(data());
	auto dx = disc->step(0);
	auto dy = disc->dim() > 1 ? disc->step(1) : 0;
	if (dx <= 0)
		dx = disc->range(0).length() / disc->size(0);
	if (dy <= 0)
		dy = disc->range(1).length() / disc->size(disc->dim() > 1 ? 1 : 0);

	auto half_dx = dx / 2;
	auto half_dy = dy / 2 ;

	for (unsigned i = 0; i < disc->size(); i++) {
		auto pt = disc->pointAt(i, 0);
		paint->setColor(mapValueToColor_(pt.back(), ch));
		paint->fillRect({ pt[0] - half_dx, pt[1] - half_dy, 0 },
			{ pt[0] + half_dx, pt[1] + half_dy, 0 });
	}

	if (showBorder() && borderPen().visible()) {
		paint->apply(borderPen());
		for (unsigned i = 0; i < disc->size(); i++) {
			auto pt = disc->pointAt(i, 0);
			paint->drawRect({ pt[0] - half_dx, pt[1] - half_dy, 0 },
				{ pt[0] + half_dx, pt[1] + half_dy, 0 });
		}
	}

	if (showText_ && clrText_.a() != 0) {
		paint->setColor(clrText_);
		auto leng = paint->projectv({ dx, -dy, 0 });
		for (unsigned i = 0; i < disc->size(); i++) {
			auto pt = disc->pointAt(i, 0);
			auto text = KuStrUtil::toString(pt.back());
			auto szText = paint->textSize(text.c_str());
			if (szText.x() <= leng.x() && szText.y() <= leng.y())
				paint->drawText({ pt[0], pt[1], 0 }, text.c_str(), 0);
		}
	}
}
