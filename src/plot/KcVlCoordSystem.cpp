#include "KcVlCoordSystem.h"
#include "KgAxis.h"
#include <vlGraphics/Applet.hpp>
#include <vlGraphics/Geometry.hpp>
#include <vlGraphics/GeometryPrimitives.hpp>
#include "KglPaint.h"


KcVlCoordSystem::KcVlCoordSystem()
	: KvCoordSystem(vec3(0, 0, 0), vec3(10, 10, 10))
{

}


void KcVlCoordSystem::draw(KglPaint* paint) const
{
	if (visible()) {
		for (int i = 0; i < 12; i++) {
			auto ax = axis(i);
			if (ax && ax->visible())
				drawAxis_(paint, ax.get());

		}
	}
}


void KcVlCoordSystem::drawAxis_(KglPaint* paint, KgAxis* axis) const
{
	assert(axis->visible());

	// draw baseline
	if (axis->showBaseline()) {
		auto clr = axis->baselineColor();
		paint->setColor(clr);
		paint->setLineWidth(axis->baselineSize()); // TODO: dock后，线的宽度会改变
		paint->drawLine(axis->start(), axis->end()); // 物理坐标
	}

	// draw ticks
	if (axis->showTick())
		drawTicks_(paint, axis);
}


void KcVlCoordSystem::drawTicks_(KglPaint* paint, KgAxis* axis) const
{
	assert(axis->showTick());

	auto aabbLen = (upperCorner() - lowerCorner()).length();
	auto ticker = axis->ticker();
	auto lower = axis->lower();
	auto upper = axis->upper();
	ticker->autoRange(lower, upper);

	// major
	auto ticks = ticker->getTicks(lower, upper);
	if (!ticks.empty()) {

		double tickLen = axis->tickLength() / aabbLen; // tick的长度取相对值
		auto clr = axis->tickColor();
		paint->setColor(clr);
		paint->setLineWidth(axis->tickSize());

		for (unsigned i = 0; i < ticks.size(); i++) {
			auto ref = (ticks[i] - axis->lower()) / axis->length();
			auto tickStart = axis->start() + (axis->end() - axis->start()) * ref; // lerp
			drawTick_(paint, axis, tickStart, tickLen);
		}
	}

	// minor
	if (axis->showSubtick()) {
		auto subticks = ticker->getSubticks(ticks);
		if (!subticks.empty()) {

			double subtickLen = axis->subtickLength() / aabbLen; // subtick的长度取相对值
			auto clr = axis->subtickColor();
			paint->setColor(clr);
			paint->setLineWidth(axis->subtickSize());

			for (unsigned i = 0; i < subticks.size(); i++) {
				auto ref = (subticks[i] - axis->lower()) / axis->length();
				auto tickStart = axis->start() + (axis->end() - axis->start()) * ref; // lerp
				drawTick_(paint, axis, tickStart, subtickLen);
			}
		}
	}
}


void KcVlCoordSystem::drawTick_(KglPaint* paint, KgAxis* axis, const vec3& pt, double length) const
{
	auto d = axis->tickOrient() * length;
	paint->drawLine(axis->tickShowBothSide() ? pt - d : pt, pt + d);
}