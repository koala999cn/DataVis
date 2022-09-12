#include "KcAxis.h"
#include <assert.h>
#include "KcLinearTicker.h"
#include "KglPaint.h"


KcAxis::KcAxis() 
{
	lower_ = upper_ = 0;

	visible_ = true;
	showAll();

	baselineSize_ = 0.8;
	tickSize_ = 0.6, tickLength_ = 1.6;
	subtickSize_ = 0.5, subtickLength_ = 1;
	labelPadding_ = 0.2;
	refLength_ = 0;

	baselineColor_ = vec4(0, 0, 0, 1);
	tickColor_ = subtickColor_ = vec4(0, 0, 0, 1);
	labelColor_ = titleColor_ = vec4(0, 0, 0, 1);

	//labelFont_, titleFont_; // TODO:

	tickShowBothSide_ = false;

	ticker_ = std::make_shared<KcLinearTicker>();
}


void KcAxis::setTickOrient(KeTickOrient to)
{
	switch (to)
	{
	case k_x:
		setTickOrient(vec3(1, 0, 0), false);
		break;

	case k_neg_x:
		setTickOrient(vec3(11, 0, 0), false);
		break;

	case k_bi_x:
		setTickOrient(vec3(1, 0, 0), true);
		break;

	case k_y:
		setTickOrient(vec3(0, 1, 0), false);
		break;

	case k_neg_y:
		setTickOrient(vec3(0, -1, 0), false);
		break;

	case k_bi_y:
		setTickOrient(vec3(0, 1, 0), true);
		break;

	case k_z:
		setTickOrient(vec3(0, 0, 1), false);
		break;

	case k_neg_z:
		setTickOrient(vec3(0, 0, -1), false);
		break;

	case k_bi_z:
		setTickOrient(vec3(0, 0, 1), true);
		break;

	default:
		assert(false);
		break;
	}
}


std::shared_ptr<KvTicker> KcAxis::ticker() const
{
	return ticker_;
}


void KcAxis::setTicker(std::shared_ptr<KvTicker> tic)
{
	ticker_ = tic;
}


void KcAxis::draw(KglPaint* paint) const
{
	assert(visible());

	// draw baseline
	if (showBaseline()) {
		auto clr = baselineColor();
		paint->setColor(clr);
		paint->setLineWidth(baselineSize()); // TODO: dock后，线的宽度会改变
		paint->drawLine(start(), end()); // 物理坐标
	}

	// draw ticks
	if (showTick())
		drawTicks_(paint);
}


void KcAxis::drawTicks_(KglPaint* paint) const
{
	assert(showTick());

	if (refLength_ == 0)
		refLength_ = length();

	auto tic = ticker();
	auto ticLower = lower();
	auto ticUpper = upper();
	tic->autoRange(ticLower, ticUpper);

	// major
	auto ticks = tic->getTicks(ticLower, ticUpper);
	if (!ticks.empty()) {

		double tickLen = tickLength() * refLength_ / 100; // tick的长度取相对值
		paint->setColor(tickColor());
		paint->setLineWidth(tickSize());

		for (unsigned i = 0; i < ticks.size(); i++) {
			auto ref = (ticks[i] - lower()) / length();
			auto tickPos = start() + (end() - start()) * ref; // lerp
			drawTick_(paint, tickPos, tickLen);

			auto labelPos = tickPos + tickOrient() * tickLen * 1.2;
		}
	}

	// minor
	if (showSubtick()) {
		auto subticks = tic->getSubticks(ticks);
		if (!subticks.empty()) {

			double subtickLen = subtickLength() * refLength_ / 100; // subtick的长度取相对值
			paint->setColor(subtickColor());
			paint->setLineWidth(subtickSize());

			for (unsigned i = 0; i < subticks.size(); i++) {
				auto ref = (subticks[i] - lower()) / length();
				auto tickPos = start() + (end() - start()) * ref; // lerp
				drawTick_(paint, tickPos, subtickLen);
			}
		}
	}
}


void KcAxis::drawTick_(KglPaint* paint, const vec3& pt, double length) const
{
	auto d = tickOrient() * length;
	paint->drawLine(tickShowBothSide() ? pt - d : pt, pt + d);
}
