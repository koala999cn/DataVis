#include "KcAxis.h"
#include <assert.h>
#include "KcLinearTicker.h"
#include "KglPaint.h"


KcAxis::KcAxis() 
{
	lower_ = upper_ = 0;

	showAll();

	baselineWidth_ = 0.8;
	tickWidth_ = 0.6, tickLength_ = 1;
	subtickWidth_ = 0.4, subtickLength_ = 0.6;
	labelPadding_ = 0.2;
	refLength_ = 0;

	baselineColor_ = color4f(0, 0, 0, 1);
	tickColor_ = subtickColor_ = color4f(0, 0, 0, 1);
	labelColor_ = titleColor_ = color4f(0, 0, 0, 1);

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
		paint->setLineWidth(baselineWidth()); // TODO: dock后，线的宽度会改变
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
		paint->setLineWidth(tickWidth());

		std::vector<vec3> labelAchors;
		if (showLabel())
			labelAchors.resize(ticks.size());

		for (unsigned i = 0; i < ticks.size(); i++) {
			auto anchor = tickPos(ticks[i]);
			drawTick_(paint, anchor, tickLen);

			if (showLabel())
				labelAchors[i] = anchor + tickOrient() * (tickLen + labelPadding_);
		}

		if (showLabel()) {
			// TODO: paint->setFont();
			paint->setColor(labelColor());
			for (unsigned i = 0; i < ticks.size(); i++) {
				auto label = i < labels_.size() ? labels_[i] : tic->label(ticks[i]);
				paint->drawText(labelAchors[i], label, labelAlignment_(tickOrient_));
			}
		}
	}

	// minor
	if (showSubtick()) {
		auto subticks = tic->getSubticks(ticks);
		if (!subticks.empty()) {

			double subtickLen = subtickLength() * refLength_ / 100; // subtick的长度取相对值
			paint->setColor(subtickColor());
			paint->setLineWidth(subtickWidth());

			for (unsigned i = 0; i < subticks.size(); i++) 
				drawTick_(paint, tickPos(subticks[i]), subtickLen);
		}
	}
}


void KcAxis::drawTick_(KglPaint* paint, const vec3& anchor, double length) const
{
	auto d = tickOrient() * length;
	paint->drawLine(tickShowBothSide() ? anchor - d : anchor, anchor + d);
}


int KcAxis::labelAlignment_(const vec3& orient)
{
	int align(0);

	if (orient.x() > 0)
		align |= vl::AlignLeft;
	else if (orient.x() < 0)
		align |= vl::AlignRight;

	if (orient.y() > 0 || orient.z() < 0 )
		align |= vl::AlignBottom;
	else if (orient.y() < 0 || orient.z() > 0)
		align |= vl::AlignTop;

	if ((align & (vl::AlignLeft | vl::AlignRight)) == 0)
		align |= vl::AlignHCenter;

	if ((align & (vl::AlignBottom | vl::AlignTop)) == 0)
		align |= vl::AlignVCenter;

	return align;
}


KcAxis::vec3 KcAxis::tickPos(double val) const
{
	auto ratio = (val - lower()) / length();
	return start() + (end() - start()) * ratio; // TODO: lerp
}