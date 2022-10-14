#include "KcAxis.h"
#include <assert.h>
#include "KcLinearTicker.h"
#include "KvPaint.h"


KcAxis::KcAxis() 
	: KvRenderable("Axis")
{
	lower_ = upper_ = 0;

	showAll();

	baselineCxt_.width = 0.8;
	tickCxt_.width = 0.6;
	tickCxt_.length = 1;
	subtickCxt_.width = 0.4;
	subtickCxt_.length = 0.6;

	labelPadding_ = 0.2;
	refLength_ = 0;

	//labelFont_, titleFont_; // TODO:

	tickShowBothSide_ = false;

	ticker_ = std::make_shared<KcLinearTicker>();
}


void KcAxis::setTickOrient(KeTickOrient to)
{
	switch (to)
	{
	case k_x:
		setTickOrient(vec3::unitX(), false);
		break;

	case k_neg_x:
		setTickOrient(-vec3::unitX(), false);
		break;

	case k_bi_x:
		setTickOrient(vec3::unitX(), true);
		break;

	case k_y:
		setTickOrient(vec3::unitY(), false);
		break;

	case k_neg_y:
		setTickOrient(-vec3::unitY(), false);
		break;

	case k_bi_y:
		setTickOrient(vec3::unitY(), true);
		break;

	case k_z:
		setTickOrient(vec3::unitZ(), false);
		break;

	case k_neg_z:
		setTickOrient(-vec3::unitZ(), false);
		break;

	case k_bi_z:
		setTickOrient(vec3::unitZ(), true);
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


KcAxis::aabb_type KcAxis::boundingBox() const
{
	return { start(), end() };
}


void KcAxis::draw(KvPaint* paint) const
{
	assert(visible());

	// draw baseline
	if (showBaseline()) {
		paint->apply(baselineCxt_);
		paint->drawLine(start(), end()); // 物理坐标
	}

	// draw ticks
	if (showTick())
		drawTicks_(paint);
}


void KcAxis::drawTicks_(KvPaint* paint) const
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

		paint->apply(tickCxt_);
		double tickLen = tickCxt_.length * refLength_ / 100; // tick的长度取相对值

		std::vector<point3> labelAchors;
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
				paint->drawText(labelAchors[i], label.c_str(), labelAlignment_(tickOrient_));
			}
		}
	}

	// minor
	if (showSubtick()) {
		auto subticks = tic->getSubticks(ticks);
		if (!subticks.empty()) {

			paint->apply(subtickCxt_);
			double subtickLen = subtickCxt_.length * refLength_ / 100; // subtick的长度取相对值

			for (unsigned i = 0; i < subticks.size(); i++) 
				drawTick_(paint, tickPos(subticks[i]), subtickLen);
		}
	}
}


void KcAxis::drawTick_(KvPaint* paint, const point3& anchor, double length) const
{
	auto d = tickOrient() * length;
	paint->drawLine(tickShowBothSide() ? anchor - d : anchor, anchor + d);
}


int KcAxis::labelAlignment_(const vec3& orient)
{
	int align(0);

	if (orient.x() > 0)
		align |= k_align_left;
	else if (orient.x() < 0)
		align |= k_align_right;

	if (orient.y() > 0 || orient.z() < 0 )
		align |= k_align_bottom;
	else if (orient.y() < 0 || orient.z() > 0)
		align |= k_align_top;

	//if ((align & (vl::AlignLeft | vl::AlignRight)) == 0)
	//	align |= vl::AlignHCenter;

	//if ((align & (vl::AlignBottom | vl::AlignTop)) == 0)
	//	align |= vl::AlignVCenter;

	return align;
}


KcAxis::point3 KcAxis::tickPos(double val) const
{
	auto ratio = (val - lower()) / length();
	return start() + (end() - start()) * ratio; // TODO: lerp
}


KcAxis::point2 KcAxis::calcSize() const
{
	point2 sz;

}