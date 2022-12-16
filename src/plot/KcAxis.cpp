#include "KcAxis.h"
#include <assert.h>
#include "KcLinearScaler.h"
#include "KvPaint.h"
#include "KtuMath.h"
#include "KtLine.h"
#include "layout/KeAlignment.h"
#include "layout/KuLayoutUtil.h"


KcAxis::KcAxis(KeType type, int dim, bool main)
	: KvRenderable("Axis")
	, type_(type)
	, dimReal_(dim)
	, dimSwapped_(dim)
	, main_(main)
{
	lower_ = upper_ = 0;

	showAll();

	baselineCxt_.width = 0.8;
	tickCxt_.width = 0.6;
	tickCxt_.length = 5;
	subtickCxt_.width = 0.4;
	subtickCxt_.length = 3;

	//labelFont_, titleFont_; // TODO:

	scaler_ = std::make_shared<KcLinearScaler>();
}

std::shared_ptr<KvScaler> KcAxis::scaler() const
{
	return scaler_;
}


void KcAxis::setScaler(std::shared_ptr<KvScaler> scale)
{
	scaler_ = scale;
}


KcAxis::aabb_t KcAxis::boundingBox() const
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

	if (length() == 0)
		return; // TODO: draw or not draw ? draw what ??

	auto scale = scaler();
	scale->generate(lower(), upper(), showSubtick(), showLabel());
	const auto& ticks = scale->ticks();

	assert(KtuMath<float_t>::almostEqual(tickOrient().length(), 1));

	// ������Ļ����1�����س߶ȣ��൱������������ٸ���λ����
	auto tl = paint->projectv(tickOrient_);
	auto ll = paint->projectv(labelOrient_);
	float_t tickLenPerPixel = 1 / tl.length();
	float_t labelPaddingPerPixel = 1 / ll.length();

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
		auto& labels = scale->labels();
		for (unsigned i = 0; i < ticks.size(); i++) {
			auto label = i < labels_.size() ? labels_[i] : labels[i];

			// ����������������ƣ�Ҳ������Ļ�������
			KeAlignment align = labelAlignment_(paint, paint->currentCoord() == KvPaint::k_coord_screen);

			paint->drawText(labelAchors[i], label.c_str(), align);
		}
	}

	// minor
	auto& subticks = scale->subticks();
	if (showSubtick() && !subticks.empty()) {
		
		paint->apply(subtickCxt_);
		double subtickLen = subtickCxt_.length * tickLenPerPixel;

		for (unsigned i = 0; i < subticks.size(); i++) 
			drawTick_(paint, tickPos(subticks[i]), subtickLen);
	}
}


void KcAxis::drawTick_(KvPaint* paint, const point3& anchor, double length) const
{
	auto d = tickOrient() * length;
	paint->drawLine(tickBothSide() ? anchor - d : anchor, anchor + d);
}


int KcAxis::labelAlignment_(KvPaint* paint, bool toggleTopBottom) const
{
	int align(0);
	auto labelOrient = paint->localToWorldV(labelOrient_); // labelOrient_

	if (labelOrient.x() > 0)
		align |= KeAlignment::k_left;
	else if (labelOrient.x() < 0)
		align |= KeAlignment::k_right;

	if (labelOrient.y() > 0 || labelOrient.z() < 0 )
		align |= toggleTopBottom ? KeAlignment::k_top : KeAlignment::k_bottom;
	else if (labelOrient.y() < 0 || labelOrient.z() > 0)
		align |= toggleTopBottom ? KeAlignment::k_bottom : KeAlignment::k_top;

	return align;
}


namespace kPrivate
{
	template<typename T1, typename T2>
	T2 remap(const T1& x, const T1& x0, const T1& x1, const T2& y0, const T2& y1, bool inv)
	{
		return !inv ? KtuMath<T1>::remap<T2>(x, x0, x1, y0, y1)
			: KtuMath<T1>::remap<T2>(x, x0, x1, y1, y0);
	}
}

KcAxis::point3 KcAxis::tickPos(double val) const
{
	return kPrivate::remap(val, lower(), upper(), start(), end(), inversed());
}


KcAxis::size_t KcAxis::calcSize_(void* cxt) const
{
	assert(visible() && length() > 0);

	auto paint = (KvPaint*)cxt;

	switch (typeReal())
	{
	case KcAxis::k_left:
		return { calcMargins(paint).left(), 0 };

	case KcAxis::k_right:
		return { calcMargins(paint).right(), 0 };

	case KcAxis::k_bottom:
		return { 0, calcMargins(paint).bottom() };

	case KcAxis::k_top:
		return { 0, calcMargins(paint).top() };

	default:
		break;
	}
	
	return { 0, 0 };
}


// calcMargins��Ҫ����������ƣ��Լ���������Ϊ��λ��marginsΪ��
// draw������������ϵ����ʵ�ʻ��ƣ�paint��ִ�л���ָ��ʱ��������ת��
KtMargins<KcAxis::float_t> KcAxis::calcMargins(KvPaint* paint) const
{
	//if (!visible() || length() == 0)
	//	return { 0, 0, 0, 0 };

	paint->pushCoord(KvPaint::k_coord_screen); // ���м�������Ļ�����½���

	auto tickOrient = paint->projectv(tickOrient_).normalize();

	vec3 tickLen(0); // tick�ĳ���ʸ��
	if (showTick())
		tickLen = tickOrient * tickCxt_.length;

	auto scale = scaler();
	scale->generate(lower(), upper(), false, showLabel());
	auto& ticks = scale->ticks();

	vec3 dir = paint->projectv((end() - start()).normalize()); // ������ķ���ʸ��
	point3 lowerPt(0), upperPt(lowerPt + dir * length()); // ����Ŀǰ��֪start��end��ʵ��ֵ����rangeΪ����������������ϵ
	aabb_t box(lowerPt, upperPt);

	if (showBaseline()) {
		auto w = baselineCxt_.width;
		if (typeReal() == k_left || typeReal() == k_bottom)
			w = -w;
		box.merge({ w, w, w });
	}

	if (showTick() && !ticks.empty())	{
		// �ϲ���һ�������һ��tick��box
		float_t pos[2];
		pos[0] = kPrivate::remap(ticks.front(), lower(), upper(), 0., length(), inversed());
		pos[1] = kPrivate::remap(ticks.back(), lower(), upper(), 0., length(), inversed());
		for (int i = 0; i < 2; i++)
			box.merge(lowerPt + dir * pos[i] + tickLen);
	}

	
	if (showLabel()) { // �ϲ���label��box

		// �ж�label��tick�Ƿ����������ͬ��
		bool sameSide = tickAndLabelInSameSide_();
		auto& labels = scale->labels();

		for (unsigned i = 0; i < ticks.size(); i++) {
			auto pos = kPrivate::remap(ticks[i], lower(), upper(), 0., length(), inversed());
			auto labelAchors = lowerPt + dir * pos;
			if (sameSide)
				labelAchors += tickOrient * tickCxt_.length;

			labelAchors += paint->projectv(labelOrient_).normalize() * labelPadding_;

			auto labelText = i < labels_.size() ? labels_[i] : labels[i];
			box.merge(textBox_(paint, labelAchors, labelText));
		}
	}

	if (showTitle()) {
		//margins += paint->textSize(title_.c_str()).x();
		//margins += titlePadding_;
	}

	aabb_t outter(box.lower(), box.upper());
	aabb_t inner(lowerPt, upperPt);
	
	auto l = inner.lower() - outter.lower();
	auto u = outter.upper() - inner.upper();
	
	KtMargins<KcAxis::float_t> margs;
	margs.left() = l.x();
	margs.right() = u.x();
	margs.bottom() = l.y(); // ���ڼ�������ʸ��������������ϵ�����Բ��ý���bottom��top
	margs.top() = u.y();
	assert(margs.geAll({ 0, 0, 0, 0 }));

	paint->popCoord();

	return margs;
}


bool KcAxis::tickAndLabelInSameSide_() const
{
	KtLine<float_t> line(point3(0), end() - start());
	auto tickSide = line.whichSide(tickOrient_);
	auto labelSide = line.whichSide(labelOrient_);
	return (tickSide * labelSide).geAll(point3(0));
}


KcAxis::aabb_t KcAxis::textBox_(KvPaint* paint, const point3& anchor, const std::string& text) const
{
	auto r = KuLayoutUtil::anchorAlignedRect({ anchor.x(), anchor.y() },
		paint->textSize(text.c_str()), labelAlignment_(paint, true));

	return { 
		{ r.lower().x(), r.lower().y(), anchor.z() }, 
		{ r.upper().x(), r.upper().y(), anchor.z() } 
	};

	// �˴�rΪ��Ļ���꣬��Ҫת��Ϊ��ͼ����
	//auto yLower = 2 * anchor.y() - r.upper().y();
	//auto yUpper = 2 * anchor.y() - r.lower().y();

	//return { { r.lower().x(), yLower, anchor.z() }, { r.upper().x(), yUpper, anchor.z() } };
}


KcAxis::KeType KcAxis::typeReal() const
{
	// ��������ͽ�������
	//constexpr static int swapType[4][3][3] = {
		/// KcAxis::k_near_left
		//{
		//	/*dim0*/ { KcAxis::k_near_left, KcAxis::k_near_bottom, -1 },
		//	/*dim1*/ { KcAxis::k_near_left, KcAxis::k_near_bottom, -1 },
		//},
		/// KcAxis::k_near_right

		/// KcAxis::k_near_bottom

		/// KcAxis::k_near_top
	//};
	
	//return swapType[type_][dimReal_][dimSwapped_];
	
	constexpr static int swapType[][4] = {
			/* swap_none */          /* swap_xy */           /* swap_xz */        /* swap_yz */
		{ KcAxis::k_near_left,   KcAxis::k_near_bottom, -1,                    KcAxis::k_ceil_left   },
		{ KcAxis::k_near_right,  KcAxis::k_near_top ,   -1,                    KcAxis::k_ceil_right  },
		{ KcAxis::k_near_bottom, KcAxis::k_near_left,   KcAxis::k_floor_right, -1                    },
		{ KcAxis::k_near_top,    KcAxis::k_near_right,  KcAxis::k_ceil_right,  -1                    },

		{ KcAxis::k_far_left,    KcAxis::k_far_bottom, -1,                     KcAxis::k_floor_left  },
		{ KcAxis::k_far_right,   KcAxis::k_far_top,    -1,                     KcAxis::k_floor_right },
		{ KcAxis::k_far_bottom,  KcAxis::k_far_left,   KcAxis::k_floor_left,   -1                    },
		{ KcAxis::k_far_top,     KcAxis::k_far_right,  KcAxis::k_ceil_left,    -1                    },

		{ KcAxis::k_floor_left,  -1,                   KcAxis::k_far_bottom,   KcAxis::k_far_left   },
		{ KcAxis::k_floor_right, -1,                   KcAxis::k_near_bottom,  KcAxis::k_far_right  },
		{ KcAxis::k_ceil_left,   -1,                   KcAxis::k_far_top,      KcAxis::k_near_left  },
		{ KcAxis::k_ceil_right,  -1,                   KcAxis::k_near_top,     KcAxis::k_near_right }
	};

	// TODO: ��ʱֻ���Ƕ�ά���
	return KeType(swapType[type_][swapped() ? 1 : 0]);
}
