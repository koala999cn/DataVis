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
		setTickOrient(vec3(-1, 0, 0), false);
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

		std::vector<vec3> labelAchors;
		if (showLabel())
			labelAchors.resize(ticks.size());

		for (unsigned i = 0; i < ticks.size(); i++) {
			auto ref = (ticks[i] - lower()) / length();
			auto tickAnchor = start() + (end() - start()) * ref; // lerp
			drawTick_(paint, tickAnchor, tickLen);

			if (showLabel())
				labelAchors[i] = tickAnchor + tickOrient() * tickLen * 1.2;
		}

		if (showLabel()) {
			// TODO: paint->setFont();
			paint->setColor(labelColor());
			for (unsigned i = 0; i < ticks.size(); i++) {
				auto label = i < labels_.size() ? labels_[i] : tic->label(ticks[i]);
				drawLabel_(paint, labelAchors[i], label);
			}
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
				auto subtickAnchor = start() + (end() - start()) * ref; // lerp
				drawTick_(paint, subtickAnchor, subtickLen);
			}
		}
	}
}


void KcAxis::drawTick_(KglPaint* paint, const vec3& anchor, double length) const
{
	auto d = tickOrient() * length;
	paint->drawLine(tickShowBothSide() ? anchor - d : anchor, anchor + d);
}


void KcAxis::drawLabel_(KglPaint* paint, const vec3& anchor, const std::string& str) const
{
	paint->drawText(vl::fvec3(anchor.x, anchor.y, anchor.z), str, vl::AlignHCenter | vl::AlignVCenter);
}

