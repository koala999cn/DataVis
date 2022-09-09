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
	// draw baseline
	if (axis->showBaseline()) {
		auto clr = axis->baselineColor();
		paint->setColor(clr);
		paint->setLineWidth(axis->baselineSize()); // TODO: dock���ߵĿ�Ȼ�ı�
		paint->drawLine(axis->start(), axis->end()); // ��������
	}

	// draw ticks
	if (axis->showTick()) {

		// major
		auto aabbLen = (upperCorner()-lowerCorner()).length();
		
		auto ticker = axis->ticker();
		auto lower = axis->lower();
		auto upper = axis->upper();
		ticker->autoRange(lower, upper);
		auto ticks = ticker->getTicks(lower, upper);
		if (!ticks.empty()) {

			double tickLen = axis->tickLength() / aabbLen; // tick�ĳ���ȡ���ֵ
			auto clr = axis->tickColor();
			paint->setColor(clr);
			paint->setLineWidth(axis->tickSize());

			for (unsigned i = 0; i < ticks.size(); i++) {
				auto ref = (ticks[i] - axis->lower()) / axis->length();
				auto tickStart = axis->start() + (axis->end() - axis->start()) * ref; // lerp
				auto tickEnd = tickStart + axis->tickOrient() * tickLen;
				paint->drawLine(tickStart, tickEnd);
			}
		}

		// minor
		if (axis->showSubtick()) {
			auto subticks = ticker->getSubticks(ticks);
			if (!subticks.empty()) {

				double subtickLen = axis->subtickLength() / aabbLen; // subtick�ĳ���ȡ���ֵ
				auto clr = axis->subtickColor();
				paint->setColor(clr);
				paint->setLineWidth(axis->subtickSize());

				for (unsigned i = 0; i < subticks.size(); i++) {
					auto ref = (subticks[i] - axis->lower()) / axis->length();
					auto tickStart = axis->start() + (axis->end() - axis->start()) * ref; // lerp
					auto tickEnd = tickStart + axis->tickOrient() * subtickLen;
					paint->drawLine(tickStart, tickEnd);
				}
			}
		}
	}
}
