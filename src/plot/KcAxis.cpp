#include "KcAxis.h"
#include <assert.h>
#include "KcLinearTicker.h"
#include "KvPaint.h"
#include "KtuMath.h"
#include "KtLine.h"


KcAxis::KcAxis(KeAxisType type)
	: KvRenderable("Axis")
	, type_(type)
{
	lower_ = upper_ = 0;

	showAll();

	baselineCxt_.width = 0.8;
	tickCxt_.width = 0.6;
	tickCxt_.length = 5;
	subtickCxt_.width = 0.4;
	subtickCxt_.length = 3;

	//labelFont_, titleFont_; // TODO:

	ticker_ = std::make_shared<KcLinearTicker>();
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
		paint->drawLine(start(), end()); // ��������
	}

	// draw ticks
	if (showTick() || showLabel())
		drawTicks_(paint);
}


void KcAxis::drawTicks_(KvPaint* paint) const
{
	assert(showTick() || showLabel());

	auto tic = ticker();
	auto ticLower = lower();
	auto ticUpper = upper();
	tic->autoRange(ticLower, ticUpper);

	// major
	auto ticks = tic->getTicks(ticLower, ticUpper);
	if (ticks.empty())
		return;

	assert(KtuMath<float_t>::almostEqual(tickOrient().length(), 1));
	auto pt0 = paint->project(point3(0));
	auto pt1 = paint->project(tickOrient_);
	auto pt2 = paint->project(labelOrient_);

	// ������Ļ����1�����س߶ȣ��൱������������ٸ���λ����
	float_t tickLenPerPixel = 1 / (pt1 - pt0).length();
	float_t labelPaddingPerPixel = 1 / (pt2 - pt0).length();

	paint->apply(tickCxt_);

	std::vector<point3> labelAchors;
	bool sameSide = tickAndLabelInSameSide_();
	if (showLabel())
		labelAchors.resize(ticks.size());

	for (unsigned i = 0; i < ticks.size(); i++) {
		auto anchor = tickPos(ticks[i]);

		if (showTick())
		    drawTick_(paint, anchor, tickCxt_.length * tickLenPerPixel);

		if (showLabel()) {
			labelAchors[i] = anchor + labelOrient_ * labelPadding_ * labelPaddingPerPixel;

			if (sameSide && showTick())
				labelAchors[i] += tickOrient_ * tickCxt_.length * tickLenPerPixel;
		}
	}

	if (showLabel()) {
		// TODO: paint->setFont();
		paint->setColor(labelColor());
		for (unsigned i = 0; i < ticks.size(); i++) {
			auto label = i < labels_.size() ? labels_[i] : tic->label(ticks[i]);
			paint->drawText(labelAchors[i], label.c_str(), labelAlignment_());
		}
	}

	// minor
	if (showSubtick()) {
		auto subticks = tic->getSubticks(ticks);
		if (!subticks.empty()) {

			paint->apply(subtickCxt_);
			double subtickLen = subtickCxt_.length * tickLenPerPixel;

			for (unsigned i = 0; i < subticks.size(); i++) 
				drawTick_(paint, tickPos(subticks[i]), subtickLen);
		}
	}
}


void KcAxis::drawTick_(KvPaint* paint, const point3& anchor, double length) const
{
	auto d = tickOrient() * length;
	paint->drawLine(tickBothSide() ? anchor - d : anchor, anchor + d);
}


int KcAxis::labelAlignment_() const
{
	int align(0);

	if (labelOrient_.x() > 0)
		align |= k_align_left;
	else if (labelOrient_.x() < 0)
		align |= k_align_right;

	if (labelOrient_.y() > 0 || labelOrient_.z() < 0 )
		align |= k_align_bottom;
	else if (labelOrient_.y() < 0 || labelOrient_.z() > 0)
		align |= k_align_top;

	return align;
}


KcAxis::point3 KcAxis::tickPos(double val) const
{
	auto ratio = (val - lower()) / length();
	return start() + (end() - start()) * ratio; // TODO: lerp
}


// calcMargins��Ҫ����������ƣ��Լ���������Ϊ��λ��marginsΪ��
// draw������������ϵ����ʵ�ʻ��ƣ�paint��ִ�л���ָ��ʱ��������ת��
KtMargins<KcAxis::float_t> KcAxis::calcMargins(KvPaint* paint) const
{
	if (!visible())
		return { 0, 0, 0, 0 };

	vec3 dir = (end() - start()).normalize(); // ������ķ���ʸ��
	point3 lowerPt(0), upperPt(lowerPt + dir * length()); // ����Ŀǰ��֪start��end��ʵ��ֵ����rangeΪ����������������ϵ
	aabb_type box(lowerPt, upperPt);

	vec3 tickLen(0); // tick�ĳ���ʸ��
	
	if (showTick())
		tickLen = tickOrient_ * tickCxt_.length;

	auto tic = ticker();
	auto ticLower = lower();
	auto ticUpper = upper();
	tic->autoRange(ticLower, ticUpper);
	auto ticks = tic->getTicks(ticLower, ticUpper);

	if (ticks.empty())
		return { 0, 0, 0, 0 };

	// �ϲ���һ�������һ��tick��box
	{
		float_t pos[2];
		pos[0] = KtuMath<float_t>::remap(ticks.front(), lower(), upper(), 0, length());
		pos[1] = KtuMath<float_t>::remap(ticks.back(), lower(), upper(), 0, length());
		for (int i = 0; i < 2; i++)
			box.merge(lowerPt + dir * pos[i] + tickLen);
	}

	
	if (showLabel()) { // �ϲ���label��box

		// �ж�label��tick�Ƿ����������ͬ��
		bool sameSide = tickAndLabelInSameSide_();

		for (unsigned i = 0; i < ticks.size(); i++) {
			auto pos = KtuMath<float_t>::remap(ticks[i], lower(), upper(), 0, length());
			auto labelAchors = lowerPt + dir * pos;
			if (sameSide)
				labelAchors += tickOrient_ * tickCxt_.length;

			labelAchors += labelOrient_ * labelPadding_;

			auto labelText = i < labels_.size() ? labels_[i] : tic->label(ticks[i]);
			box.merge(textBox_(paint, labelAchors, labelText));
		}
	}

	if (showTitle()) {
		//margins += paint->textSize(title_.c_str()).x();
		//margins += titlePadding_;
	}

	KtMargins<float_t> margins;
	margins.left() = -box.lower().x();
	margins.bottom() = -box.lower().y();
	margins.right() = box.upper().x() - upperPt.x();
	margins.top() = box.upper().y() - upperPt.y();
	assert(margins.geAll({ 0, 0, 0, 0 }));

	return margins;
}


bool KcAxis::tickAndLabelInSameSide_() const
{
	KtLine<float_t> line(point3(0), end() - start());
	auto tickSide = line.whichSide(tickOrient_);
	auto labelSide = line.whichSide(labelOrient_);
	return (tickSide * labelSide).geAll(point3(0));
}


KcAxis::aabb_type KcAxis::textBox_(KvPaint* paint, const point3& anchor, const std::string& text) const
{
	auto r = paint->textRect({ anchor.x(), anchor.y() }, text.c_str(), labelAlignment_());

	// �˴�rΪ��Ļ���꣬��Ҫת��Ϊ��ͼ����
	auto yLower = 2 * anchor.y() - r.upper().y();
	auto yUpper = 2 * anchor.y() - r.lower().y();

	return { { r.lower().x(), yLower, anchor.z() }, { r.upper().x(), yUpper, anchor.z() } };
}