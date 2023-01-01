#include "KcAxis.h"
#include <assert.h>
#include "KcLinearTicker.h"
#include "KvPaint.h"
#include "KtuMath.h"
#include "KtLine.h"
#include "KtMatrix3.h"
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


void KcAxis::draw_(KvPaint* paint, bool calcBox) const
{
	assert(visible());

	if (calcBox)
		box_ = aabb_t(start(), end());

	// draw baseline
	if (showBaseline() && baselineCxt_.style != KpPen::k_none && !calcBox) {
		paint->apply(baselineCxt_);
		paint->drawLine(start(), end()); // ��������
	}

	auto realShowTitle = showTitle() && !title().empty();

	// tickOrient_��labelOrient_ֻ����һ��
	if (realShowTitle || showTick() || showLabel()) {
		// TODO: if (calcBox) plot3d��ʱ�򲻻���ü���ģʽ
		tickOrient_ = calcTickOrient_(paint);
		if (paint->currentCoord() == KvPaint::k_coord_screen)
			tickOrient_.y() *= -1; // TODO: ���޸��õķ���
		labelOrient_ = tickCxt_.side == k_inside ? -tickOrient_ : tickOrient_;
	}


	// draw ticks & label
	if (showTick() || showLabel())
		drawTicks_(paint, calcBox);
	else if (realShowTitle) 
		titleAnchor_ = (start() + end()) / 2 + labelOrient_ * titlePadding_ / paint->projectv(labelOrient_).length();

	// draw title
	if (realShowTitle) {
		paint->setColor(titleColor());
		drawLabel_(paint, title_, titleAnchor_, calcBox);
	}
}


KcAxis::vec3 KcAxis::calcTickOrient_(KvPaint* paint) const
{
	// 12���������Ĭ��������
	static const vec3 baseOrient[] = {
		-KcAxis::vec3::unitX(), // k_near_left
		KcAxis::vec3::unitX(),  // k_near_right
		-KcAxis::vec3::unitY(), // k_near_bottom
		KcAxis::vec3::unitY(),  // k_near_top

		-KcAxis::vec3::unitX(), // k_far_left
		KcAxis::vec3::unitX(),  // k_far_right
		-KcAxis::vec3::unitY(), // k_far_bottom
		KcAxis::vec3::unitY(),  // k_far_top

		-KcAxis::vec3::unitX(), // k_floor_left
		KcAxis::vec3::unitX(),  // k_floor_right
		-KcAxis::vec3::unitX(), // k_ceil_left
		KcAxis::vec3::unitX()   // k_ceil_right
	};

	// ������̬��תyaw��pitch���任����������ϵ������ת��

	vec3 orient = (tickCxt_.side == k_inside) ? -baseOrient[typeReal()] : baseOrient[typeReal()];
	// �˴���orientΪ��������
	
	auto vAxis = paint->localToWorldV(end() - start()).getNormalize(); // �����᷽��ʸ��
	KtMatrix3<float_t> mat;
	mat.fromAngleAxis(tickCxt_.pitch, vAxis); // ����������תpitch����
	orient = mat * orient;
	
	auto vPrep = orient.cross(vAxis); // �̶��ߺ�������Ĵ�ֱʸ��
	mat.fromAngleAxis(tickCxt_.yaw, vPrep.getNormalize());
	orient = mat * orient;

	return paint->worldToLocalV(orient).getNormalize(); // �任�ؾֲ�����ϵ
}


void KcAxis::drawTicks_(KvPaint* paint, bool calcBox) const
{
	assert(showTick() || showLabel());

	if (length() == 0)
		return; // TODO: draw or not draw ? draw what ??

	ticker()->generate(lower(), upper(), showSubtick(), showLabel());
	const auto& ticks = ticker()->ticks();


	// ������Ļ����1�����س߶ȣ��൱������������ٸ���λ����
	auto tl = paint->projectv(tickOrient_);
	auto ll = paint->projectv(labelOrient_);
	float_t tickLenPerPixel = 1 / tl.length();
	float_t labelPaddingPerPixel = 1 / ll.length();

	if (!calcBox)
	    paint->apply(tickCxt_);

	std::vector<point3> labelAnchors;
	bool sameSide = tickAndLabelInSameSide_();
	if (showLabel())
		labelAnchors.resize(ticks.size());

	if (showTitle()) {
		titleAnchor_ = (start() + end()) / 2 + labelOrient_ * titlePadding_ * labelPaddingPerPixel;

		if (sameSide && showTick())
			titleAnchor_ += tickOrient_ * tickCxt_.length * tickLenPerPixel;
	}

	for (unsigned i = 0; i < ticks.size(); i++) {
		auto anchor = tickPos(ticks[i]);

		if (showTick()) 
			drawTick_(paint, anchor, tickCxt_.length * tickLenPerPixel, calcBox);

		if (showLabel()) {
			labelAnchors[i] = anchor + labelOrient_ * labelPadding_ * labelPaddingPerPixel;

			if (sameSide && showTick())
				labelAnchors[i] += tickOrient_ * tickCxt_.length * tickLenPerPixel;
		}
	}

	if (showLabel()) {

		// TODO: paint->setFont();
		paint->setColor(labelColor());
		auto& labels = ticker()->labels();
		point2 maxLabelSize(0);
		for (unsigned i = 0; i < ticks.size(); i++) {
			auto label = i < labels_.size() ? labels_[i] : labels[i];
			drawLabel_(paint, label, labelAnchors[i], calcBox);

			if (showTitle())
				maxLabelSize = point2::ceil(maxLabelSize, paint->textSize(label.c_str()));
		}

		if (showTitle() || calcBox) {

			vec3 h = vec3::unitX() * maxLabelSize.x(); // TODO: hDir
			vec3 v = vec3::unitY() * maxLabelSize.y(); // TODO: vDir
			
			h = h.projectedTo(ll);
			v = v.projectedTo(ll);
		
			auto maxSqLen = std::max(h.squaredLength(), v.squaredLength());
		
			titleAnchor_ += labelOrient_ * (std::sqrt(maxSqLen) + labelPadding_ ) * labelPaddingPerPixel;
		}
	}

	// minor
	auto& subticks = ticker()->subticks();
	if (showSubtick() && !subticks.empty()) {
		
		paint->apply(subtickCxt_);
		double subtickLen = subtickCxt_.length * tickLenPerPixel;

		for (unsigned i = 0; i < subticks.size(); i++) 
			drawTick_(paint, tickPos(subticks[i]), subtickLen, calcBox);
	}
}


void KcAxis::drawTick_(KvPaint* paint, const point3& anchor, double length, bool calcBox) const
{
	auto d = tickOrient_ * length;
	if (!calcBox)
		paint->drawLine(tickCxt_.side == k_bothside ? anchor - d : anchor, anchor + d);
	else {
		box_.merge(anchor + d);
		if (tickCxt_.side == k_bothside)
			box_.merge(anchor - d);
	}
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
	assert(visible() && length() > 0 && dimReal_ < 2);

	auto paint = (KvPaint*)cxt;

	switch (typeReal())
	{
	case KcAxis::k_left:
		return { std::max<float_t>(calcMargins(paint).left(), baselineCxt_.width), 0 };

	case KcAxis::k_right:
		return { std::max<float_t>(calcMargins(paint).right(), baselineCxt_.width), 0 };

	case KcAxis::k_bottom:
		return { 0, std::max<float_t>(calcMargins(paint).bottom(), baselineCxt_.width) };

	case KcAxis::k_top:
		return { 0, std::max<float_t>(calcMargins(paint).top(), baselineCxt_.width) };

	default:
		break;
	}
	
	return { 0, 0 };
}


// calcMargins��Ҫ����������ƣ��Լ���������Ϊ��λ��marginsΪ��
// draw������������ϵ����ʵ�ʻ��ƣ�paint��ִ�л���ָ��ʱ��������ת��
KtMargins<KcAxis::float_t> KcAxis::calcMargins(KvPaint* paint) const
{
	if (!visible() || length() == 0)
		return { 0, 0, 0, 0 };

	draw_(paint, true);

	aabb_t ibox(paint->projectp(start()), paint->projectp(end()));
	aabb_t obox(paint->projectp(box_.lower()), paint->projectp(box_.upper()));
	auto l = ibox.lower() - obox.lower();
	auto u = obox.upper() - ibox.upper();

	KtMargins<KcAxis::float_t> margs;
	margs.left() = l.x();
	margs.right() = u.x();
	margs.bottom() = u.y();
	margs.top() = l.y();
	assert(margs.ge({ 0, 0, 0, 0 }));

	return margs;
}


bool KcAxis::tickAndLabelInSameSide_() const
{
	KtLine<float_t> line(point3(0), end() - start());
	auto tickSide = line.whichSide(tickOrient_);
	auto labelSide = line.whichSide(labelOrient_);
	return (tickSide * labelSide).ge(point3(0));
}


KcAxis::KeType KcAxis::typeReal() const
{
	enum KeSwapKind
	{
		swap_none,
		swap_xy,
		swap_xz,
		swap_yz
	};

	// ����dimReal_��dimSwapped_��ȡswapKind
	// ʹ��[dimReal_][dimSwapped_ + 1]����
	constexpr static int swapKind[3][4] = {
		{ swap_yz, swap_none, swap_xy, swap_xz },
		{ swap_xz, swap_xy, swap_none, swap_yz },
		{ swap_xy, swap_xz, swap_yz, swap_none }
	};
	
	// NB����������ά�ȵ������ύ����Ҳ��Ӱ�쵱ǰ������ķ�λ��Ŀǰ��δʵ�ֽ�������µķ�λһ����
	constexpr static int swapType[][4] = {
			/* swap_none */          /* swap_xy */           /* swap_xz */        /* swap_yz */
		{ KcAxis::k_near_left,   KcAxis::k_near_bottom, KcAxis::k_far_right,   KcAxis::k_ceil_left   },
		{ KcAxis::k_near_right,  KcAxis::k_near_top ,   KcAxis::k_far_left,    KcAxis::k_ceil_right  },
		{ KcAxis::k_near_bottom, KcAxis::k_near_left,   KcAxis::k_floor_right, KcAxis::k_far_top     },
		{ KcAxis::k_near_top,    KcAxis::k_near_right,  KcAxis::k_ceil_right,  KcAxis::k_far_bottom  },

		{ KcAxis::k_far_left,    KcAxis::k_far_bottom,  KcAxis::k_near_right,  KcAxis::k_floor_left  },
		{ KcAxis::k_far_right,   KcAxis::k_far_top,     KcAxis::k_near_left,   KcAxis::k_floor_right },
		{ KcAxis::k_far_bottom,  KcAxis::k_far_left,    KcAxis::k_floor_left,  KcAxis::k_near_top    },
		{ KcAxis::k_far_top,     KcAxis::k_far_right,   KcAxis::k_ceil_left,   KcAxis::k_near_bottom },

		{ KcAxis::k_floor_left,  KcAxis::k_ceil_right,  KcAxis::k_far_bottom,  KcAxis::k_far_left    },
		{ KcAxis::k_floor_right, KcAxis::k_ceil_left,   KcAxis::k_near_bottom, KcAxis::k_far_right   },
		{ KcAxis::k_ceil_left,   KcAxis::k_floor_right, KcAxis::k_far_top,     KcAxis::k_near_left   },
		{ KcAxis::k_ceil_right,  KcAxis::k_floor_left,  KcAxis::k_near_top,    KcAxis::k_near_right  }
	};
	
	if (dimReal_ == -1) return type_; // color-bar�������ᣬ�޽���

	auto t = swapType[type_][swapKind[dimReal_][dimSwapped_ + 1]];
	return KeType(t);
}


// TODO: ���õ�ʵ�ַ�����
void KcAxis::calcLabelPos_(KvPaint* paint, const std::string_view& label, const point3& anchor, point3& topLeft, point3& hDir, point3& vDir) const
{
	auto textBox = paint->textSize(label.data());
	if (labelLayout_ == k_vert_left || labelLayout_ == k_vert_right)
		std::swap(textBox.x(), textBox.y());

	if (labelBillboard_) { // ������ģʽ������ʼ��˳��+x����չ

		bool toggle = paint->currentCoord() == KvPaint::k_coord_screen || paint->currentCoord() == KvPaint::k_coord_local_screen;
		KeAlignment align = labelAlignment_(paint, toggle);

		auto anchorInScreen = paint->projectp(anchor);
		auto rc = KuLayoutUtil::anchorAlignedRect({ anchorInScreen.x(), anchorInScreen.y() }, textBox, align);
		point3 topLeftInScreen;

		switch (labelLayout_)
		{
		case KcAxis::k_horz_top: // ȱʡ����
			topLeftInScreen = point3(rc.lower().x(), rc.lower().y(), anchorInScreen.z()); 
			hDir = paint->unprojectv(vec3::unitX());
			vDir = paint->unprojectv(vec3::unitY());
			break;

		case KcAxis::k_horz_bottom: // ���µߵ���topLeft���������½�
			topLeftInScreen = point3(rc.upper().x(), rc.upper().y(), anchorInScreen.z());
			hDir = paint->unprojectv(-vec3::unitX());
			vDir = paint->unprojectv(-vec3::unitY());
			break;

		case KcAxis::k_vert_left: // ���沼�֣�topLeft���������½�
			topLeftInScreen = point3(rc.lower().x(), rc.upper().y(), anchorInScreen.z());
			hDir = paint->unprojectv(-vec3::unitY());
			vDir = paint->unprojectv(vec3::unitX());
			break;

		case KcAxis::k_vert_right:// ���沼�֣�topLeft���������Ͻ�
			topLeftInScreen = point3(rc.upper().x(), rc.lower().y(), anchorInScreen.z());
			hDir = paint->unprojectv(vec3::unitY());
			vDir = paint->unprojectv(-vec3::unitX());
			break;

		default:
			break;
		}

		topLeft = paint->unprojectp(topLeftInScreen);
	}
	else {
		hDir = (end() - start()).getNormalize();
		if (dimSwapped_ > 0) hDir *= -1; // TODO: swap_yzģʽ�£����ֻ��Ƿ���
		vDir = labelOrient_;
		topLeft = anchor - hDir * (textBox.x() / 2) / paint->projectv(hDir).length();
	}
}


void KcAxis::drawLabel_(KvPaint* paint, const std::string_view& label, const point3& anchor, bool calcBox) const
{
	point3 topLeft, hDir, vDir;
	calcLabelPos_(paint, label.data(), anchor, topLeft, hDir, vDir);
	if (!calcBox) {
		paint->drawText(topLeft, hDir, vDir, label.data());
	}
	else {
		auto sz = paint->textSize(label.data());
		auto h = paint->projectv(hDir) * sz.x();
		auto v = paint->projectv(vDir) * sz.y();
		h = paint->unprojectv(h);
		v = paint->unprojectv(v);
		box_.merge({ topLeft, topLeft + h + v });
	}
}
