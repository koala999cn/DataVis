#include "KcAxis.h"
#include <assert.h>
#include "KcLinearTicker.h"
#include "KvPaint.h"
#include "KuMath.h"
#include "KtLine.h"
#include "KtQuaternion.h"
#include "layout/KeAlignment.h"
#include "layout/KuLayoutUtil.h"


KcAxis::KcAxis(const KcAxis& axis)
	: KvRenderable(axis.name())
	, KvLayoutElement(axis)
{
	visible() = axis.visible();

	type_ = axis.type_;
	title_ = axis.title_;
	labels_ = axis.labels_; 
	lower_ = axis.lower_, upper_ = axis.upper_; 
	showBaseline_ = axis.showBaseline_, showTick_ = axis.showTick_, showSubtick_ = axis.showSubtick_,
		showTitle_ = axis.showTitle_, showLabel_ = axis.showLabel_;

	baselineCxt_ = axis.baselineCxt_;
	tickCxt_ = axis.tickCxt_, subtickCxt_ = axis.subtickCxt_;

	labelPadding_ = axis.labelPadding_;
	labelCxt_ = axis.labelCxt_;

	titlePadding_ = axis.titlePadding_;
	titleCxt_ = axis.titleCxt_;

	start_ = axis.start_, end_ = axis.end_;
	offset_ = axis.offset_;

	ticker_ = axis.ticker_;

	dimReal_ = axis.dimReal_;
	dimSwapped_ = axis.dimSwapped_;
	main_ = axis.main_;
	inv_ = axis.inv_;

	tickOrient_ = axis.tickOrient_;
	labelOrient_ = axis.labelOrient_;
	labelSize_ = axis.labelSize_;
	titleSize_ = axis.titleSize_;
	titleAnchor_ = axis.titleAnchor_;
	hDir_ = axis.hDir_, vDir_ = axis.vDir_;
	box_ = axis.box_;
}


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


bool KcAxis::offsetOutward_() const
{
	return (typeReal() == k_left || typeReal() == k_bottom) ?
		offset_[0] < 0 : offset_[0] > 0;
}


void KcAxis::fixExtent_(KvPaint* paint) const
{
	if (iRect_.volume() == 0 || // ����plot3d
		dimReal_ == -1) // �������������ᣨ��colorbar��
		return;
	
	if (main_ && offset_ == point2(0)) // ��ƫ�Ƶ��������ᣬ��������
		return;

	assert(dimReal_ < 2);
	
	if (offsetOutward_() || offset_[0] == 0) { // �����ƫ��
		auto d = 1 - dimReal_;
		auto f = KuMath::remap(realStart()[d], box_.lower()[d], box_.upper()[d], 0., 1.);
		
		auto st = iRect_.lower(), ed = iRect_.upper();

		// NB: ��������Ҫʹ��orect�����������������ܳ���ȱ�ڣ�#I6SRQH��
		st[dimSwapped_] = oRect_.lower()[dimSwapped_], ed[dimSwapped_] = oRect_.upper()[dimSwapped_];

		std::swap(st.y(), ed.y()); // ��st�任�����½ǣ�ed�任�����Ͻǣ��Ա�����������ϵ����

		d = 1 - dimSwapped_; // ����Ļ����ϵ�£�ʹ��dimSwapped_
		st[d] = ed[d] = KuMath::lerp(st[d], ed[d], f);
		start_ = paint->unprojectp(st);
		end_ = paint->unprojectp(ed);

		d = 1 - dimReal_; // �������������Ҫ��dimReal_
		start_[d] -= offset_[0], end_[d] -= offset_[0];
	}
}


void KcAxis::draw_(KvPaint* paint, bool calcBox) const
{
	assert(visible());

	// ����layout�������������������start & end��ȷ����������������ȷ��λ
	if (!calcBox)
	    fixExtent_(paint);

	// NB: ����calcBox�Ƿ�Ϊtrue���������¼���box_
	// ��Ϊ���㲼�֣�calcBoxΪtrue������ʵ���ƣ�calcBoxΪfalse��ʱ��
	// �任�����ջ���ܲ�ͬ���ر��Ǻ��߿�����ѹ����scale����
	// �⵼��ǰ�ڼ����box_������������������صĳ��Ⱥ�λ�ò�����

	box_ = aabb_t(realStart(), realEnd()); // NB: ���ܳ�ʼ��Ϊpoint(0)��������ֻ��ʾtitleʱ���ֶ�λ����
	                                       // NB: ������setExtent, ��Ϊstart��һ����С��end

	// draw baseline
	if (showBaseline() && baselineCxt_.style != KpPen::k_none) {
		if (!calcBox) {
			paint->apply(baselineCxt_);
			paint->drawLine(realStart(), realEnd()); // ��������
		}

		// TODO: ����������Ŀ��
		// box_.inflate();
	}

	auto realShowTitle = showTitle() && !title().empty();

	if (realShowTitle || showTick() || showLabel()) {
		//if (calcBox) {
			calcTickOrient_(paint);
			calcLabelOrient_(paint);
		//}
	}


	// draw ticks & label
	if (showTick() || showSubtick() || showLabel())
		drawTicks_(paint, calcBox);

	// draw title
	if (realShowTitle) {
		
		if (!calcBox)
			paint->setColor(titleContext().color);

		calcTitleAnchor_(paint); 
		drawText_(paint, title_, titleCxt_, titleAnchor_, calcBox);
	}
}


double KcAxis::transform(double val) const
{
	return KuMath::remap(ticker_->map(val), 
		ticker_->map(lower()), ticker_->map(upper()),
		lower(), upper());
}


KcAxis::vec3 KcAxis::outsideOrient_(KvPaint* paint) const
{
	// 12���������Ĭ��������
	static const vec3 outsideOrient[] = {
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

	auto o = outsideOrient[type()]; // ԭʼ���ֲ�������ϵ�µ�ʸ�����򣬲�ʹ��typeReal()

	// NB: �����������ڻ��Ƶ�ʱ�����localΪ�գ�����updateLayout��ʱ��local�任����Ȼ���ڣ����Դ˴���һ�ж�
	//if (dimReal_ != -1) // dimReal_ == -1�������������ᣬ��ʼ��ʹ���������꣬���任
	//   o = paint->localToWorldV(o); // ���������ύ�����

	if (paint->inScreenCoord())
		o.y() *= -1; // ��Ļ����ϵ�е�y�ᷭת

	return o;
}


KcAxis::vec3 KcAxis::insideOrient_(KvPaint* paint) const
{
	return -outsideOrient_(paint);
}


KcAxis::vec3 KcAxis::axisOrient_() const
{
	return (end() - start()).normalize();
}


void KcAxis::calcTickOrient_(KvPaint* paint) const
{
	vec3 tickOrient = (tickCxt_.side == k_inside) ? insideOrient_(paint) : outsideOrient_(paint);

	assert(tickOrient.dot(axisOrient_()) == 0);

	auto vAxis = axisOrient_(); // �����᷽��ʸ��
	if (tickCxt_.pitch != 0) {
		KtQuaternion<float_t> quatPitch(tickCxt_.pitch, vAxis); // ����������תpitch����
		tickOrient = quatPitch * tickOrient;
	}
	
	if (tickCxt_.yaw != 0) {
		auto vPrep = tickOrient.cross(vAxis).normalize(); // �̶��ߺ�������Ĵ�ֱʸ��
		KtQuaternion<float_t> quatYaw(tickCxt_.yaw, vPrep);
		tickOrient = quatYaw * tickOrient;
	}

	tickOrient_ = tickOrient.normalize(); 
}


void KcAxis::calcLabelOrient_(KvPaint* paint) const
{
	labelOrient_ = tickCxt_.side == k_inside ? -tickOrient_ : tickOrient_;

	// ��labelOrient_�任Ϊ��ֱ��axis�ķ���
	auto axisOrient = axisOrient_();
	labelOrient_ = axisOrient.cross(labelOrient_);
	labelOrient_ = labelOrient_.cross(axisOrient).normalize();

	// NB:ȷ��labelʼ�ճ���ࣨ��tickOrient��axisOrientƽ��ʱ��labelOrient_����ָ���ڲࣩ
	if (labelOrient_.dot(outsideOrient_(paint)) < 0)
		labelOrient_ *= -1;

	if (labelCxt_.pitch != 0) {
		KtMatrix3<float_t> mat;
		mat.fromAngleAxis(labelCxt_.pitch, axisOrient);
		labelOrient_ = mat * labelOrient_;
	}
}


KcAxis::float_t KcAxis::orientScale_(KvPaint* paint, const vec3& o)
{
	assert(KuMath::almostEqual<float_t>(o.length(), 1.0));
	auto v = paint->projectv(o).length();
	return v > 0 ? 1. / v : 0;
}


void KcAxis::drawTicks_(KvPaint* paint, bool calcBox) const
{
	if (length() == 0)
		return; // TODO: draw or not draw ? draw what ??

	ticker()->generate(lower(), upper(), showSubtick(), showLabel());
	const auto& ticks = ticker()->ticks();


	// ������Ļ����1�����س߶ȣ��൱������������ٸ���λ����
	float_t tickOrientScale = orientScale_(paint, tickOrient_);
	float_t labelOrientScale = orientScale_(paint, labelOrient_);

	if (!calcBox)
	    paint->apply(tickCxt_);

	std::vector<point3> labelAnchors;
	bool sameSide = tickAndLabelInSameSide_();
	if (showLabel())
		labelAnchors.resize(ticks.size());

	for (unsigned i = 0; i < ticks.size(); i++) {
		auto anchor = tickPos(ticks[i]);

		if (showTick()) 
			drawTick_(paint, anchor, tickCxt_.length * tickOrientScale, calcBox);

		if (showLabel()) {
			labelAnchors[i] = anchor + labelOrient_ * labelPadding_ * labelOrientScale;

			if (sameSide && showTick())
				labelAnchors[i] += tickOrient_ * tickCxt_.length * tickOrientScale;
		}
	}

	if (showLabel()) {

		// TODO: paint->setFont();
		paint->setColor(labelContext().color);
		auto& labels = ticker()->labels();
		for (unsigned i = 0; i < ticks.size(); i++) {
			auto label = i < labels_.size() ? labels_[i] : labels[i];
			drawText_(paint, label, labelCxt_, labelAnchors[i], calcBox);
			//paint->setPointSize(3);
			//paint->drawPoint(labelAnchors[i]); // for debug
		}
	}

	// minor
	auto& subticks = ticker()->subticks();
	if (showSubtick() && !subticks.empty()) {
		
		paint->apply(subtickCxt_);
		double subtickLen = subtickCxt_.length * tickOrientScale;

		for (unsigned i = 0; i < subticks.size(); i++) 
			drawTick_(paint, tickPos(subticks[i]), subtickLen, calcBox);
	}
}


void KcAxis::drawTick_(KvPaint* paint, const point3& anchor, double length, bool calcBox) const
{
	auto d = tickOrient_ * length;
	if (!calcBox)
		paint->drawLine(tickCxt_.side == k_bothside ? anchor - d : anchor, anchor + d);
	//else {
		box_.merge(anchor + d);
		if (tickCxt_.side == k_bothside)
			box_.merge(anchor - d);
	//}
}


int KcAxis::labelAlignment_(KvPaint* paint) const
{
	auto axisOrient = paint->projectv(axisOrient_());
	auto labelOrient = paint->projectv(labelOrient_); // TODO: ����label��title

	if (std::abs(axisOrient.x()) < std::abs(axisOrient.y())) {
		return labelOrient.x() > 0 ? KeAlignment::k_left : KeAlignment::k_right;
	}
	else {
		return labelOrient.y() > 0 ? KeAlignment::k_top : KeAlignment::k_bottom;
	}
}


namespace kPrivate
{
	template<typename T1, typename T2>
	T2 remap(const T1& x, const T1& x0, const T1& x1, const T2& y0, const T2& y1, bool inv)
	{
		return !inv ? KuMath::remap<T1, T2>(x, x0, x1, y0, y1)
			: KuMath::remap<T1, T2>(x, x0, x1, y1, y0);
	}
}


KcAxis::point3 KcAxis::tickPos(double val) const
{
	return kPrivate::remap(transform(val), lower(), upper(), realStart(), realEnd(), inversed());
}


KcAxis::size_t KcAxis::calcSize_(void* cxt) const
{
	assert(visible());
	size_t sz(0, 0);

	if (length() > 0) {

		auto paint = (KvPaint*)cxt;
		auto marg = calcMargins(paint);
		margins_t m{ point2(0), point2(0) };
		
		auto off = paint->projectv({ offset_[0], offset_[0], 0 });
		constexpr auto minSize = 1e-6; // һ����С�ĳߴ�ֵ����ֹ�ߴ�Ϊ0

		switch (typeReal())
		{
		case KcAxis::k_left:
			m.lower().y() = marg.top(), m.upper().y() = marg.bottom();
			sz.x() = std::max<float_t>(marg.left(), baselineCxt_.width);

			if (off.x() < 0)
				m.upper().x() = -off.x();
			else
				sz.x() = KuMath::clampFloor(sz.x() - off.x(), minSize);

			break;

		case KcAxis::k_right:
			m.lower().y() = marg.top(), m.upper().y() = marg.bottom();
			sz.x() = std::max<float_t>(marg.right(), baselineCxt_.width);

			if (off.x() > 0)
				m.lower().x() = off.x();
			else
			    sz.x() = KuMath::clampFloor(sz.x() + off.x(), minSize);

			break;

		case KcAxis::k_bottom:
			m.lower().x() = marg.left(), m.upper().x() = marg.right();
			sz.y() = std::max<float_t>(marg.bottom(), baselineCxt_.width);

			if (off.y() > 0)
				m.lower().y() = off.y();
			else
			    sz.y() = KuMath::clampFloor(sz.y() + off.y(), minSize);

			break;

		case KcAxis::k_top:
			m.lower().x() = marg.left(), m.upper().x() = marg.right();
			sz.y() = std::max<float_t>(marg.top(), baselineCxt_.width);

			if (off.y() < 0)
				m.upper().y() = -off.y();
			else
			    sz.y() = KuMath::clampFloor(sz.y() - off.y(), minSize);

			break;

		default:
			break;
		}

		const_cast<KcAxis*>(this)->margins() = m;
	}

	return sz;
}


// calcMargins��Ҫ����������ƣ��Լ���������Ϊ��λ��marginsΪ��
// draw������������ϵ����ʵ�ʻ��ƣ�paint��ִ�л���ָ��ʱ��������ת��
KtMargins<KcAxis::float_t> KcAxis::calcMargins(KvPaint* paint) const
{
	if (!visible() || length() == 0)
		return { 0, 0, 0, 0 };

	draw_(paint, true);

	// �ֲ�����ת������Ļ���꣬�Է���������׵�����ֵ
	aabb_t ibox(paint->projectp(realStart()), paint->projectp(realEnd()));
	aabb_t obox(paint->projectp(box_.lower()), paint->projectp(box_.upper()));

	auto l = ibox.lower() - obox.lower();
	auto u = obox.upper() - ibox.upper();

	KtMargins<KcAxis::float_t> margs;
	margs.left() = l.x();
	margs.right() = u.x();
	margs.bottom() = u.y();
	margs.top() = l.y();
	//assert(margs.ge({ 0, 0, 0, 0 }));

	return margs;
}


bool KcAxis::tickAndLabelInSameSide_() const
{
	KtLine<float_t> line(point3(0), axisOrient_());
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
		{ KcAxis::k_near_right,  KcAxis::k_near_top ,   KcAxis::k_near_right,  KcAxis::k_ceil_right  },
		{ KcAxis::k_near_bottom, KcAxis::k_near_left,   KcAxis::k_floor_right, KcAxis::k_far_top     },
		{ KcAxis::k_near_top,    KcAxis::k_near_right,  KcAxis::k_ceil_right,  KcAxis::k_near_top  },

		{ KcAxis::k_far_left,    KcAxis::k_far_bottom,  KcAxis::k_far_left,    KcAxis::k_floor_left  },
		{ KcAxis::k_far_right,   KcAxis::k_far_top,     KcAxis::k_near_left,   KcAxis::k_floor_right },
		{ KcAxis::k_far_bottom,  KcAxis::k_far_left,    KcAxis::k_floor_left,  KcAxis::k_far_bottom    },
		{ KcAxis::k_far_top,     KcAxis::k_far_right,   KcAxis::k_ceil_left,   KcAxis::k_near_bottom },

		{ KcAxis::k_floor_left,  KcAxis::k_floor_left,  KcAxis::k_far_bottom,  KcAxis::k_far_left    },
		{ KcAxis::k_floor_right, KcAxis::k_ceil_left,   KcAxis::k_near_bottom, KcAxis::k_far_right   },
		{ KcAxis::k_ceil_left,   KcAxis::k_floor_right, KcAxis::k_far_top,     KcAxis::k_near_left   },
		{ KcAxis::k_ceil_right,  KcAxis::k_ceil_right,  KcAxis::k_near_top,    KcAxis::k_near_right  }
	};
	
	if (dimReal_ == -1) return type_; // color-bar�������ᣬ�޽���

	auto t = swapType[type_][swapKind[dimReal_][dimSwapped_ + 1]];
	return KeType(t);
}


void KcAxis::calcTextPos_(KvPaint* paint, const std::string_view& label, const KpTextContext& cxt, 
	const point3& anchor, point3& topLeft, vec3& hDir, vec3& vDir) const
{
	auto textBox = paint->textSize(label.data());
	if (cxt.layout == k_vert_left || cxt.layout == k_vert_right)
		std::swap(textBox.x(), textBox.y());
	
	if (cxt.billboard) { // ������ģʽ������ʼ��˳��+x����չ

		hDir = paint->unprojectv(vec3::unitX()).normalize();
		vDir = paint->unprojectv(vec3::unitY()).normalize();

		auto align = labelAlignment_(paint);
		auto anchorInScreen = paint->projectp(anchor);
		auto rc = KuLayoutUtil::anchorAlignedRect({ anchorInScreen.x(), anchorInScreen.y() }, textBox, align);
		topLeft = paint->unprojectp({ rc.lower().x(), rc.lower().y(), anchorInScreen.z() });
	}
	else {
		vDir = labelOrient_;
		hDir = axisOrient_();

		vec3 h = paint->projectv(hDir);
		vec3 v = paint->projectv(vDir);
		auto zDir = h.cross(v);
		if (zDir.z() < 0)
			hDir *= -1; // ����hDir��ȷ����������ά�ռ�Ŀɶ���

		topLeft = anchor - hDir * (textBox.x() / 2) * orientScale_(paint, hDir);
	}

	// fixTextLayout_��ҪtextBoxΪ��������ߴ磬�˴����б任
	textBox *= point2(orientScale_(paint, hDir), orientScale_(paint, vDir));
	fixTextLayout_(cxt.layout, textBox, topLeft, hDir, vDir);

	fixTextRotation_(cxt, anchor, topLeft, hDir, vDir);

	if (cxt.billboard && cxt.yaw) {
		// ����vDir��ȷ����hDir����Ļ����ϵ�´�ֱ
		vec3 hDirS = paint->projectv(hDir);
		vec3 vDirS = paint->projectv(vDir);
		vDirS = hDirS.cross(vDirS).cross(hDirS);
		vDir = paint->unprojectv(vDirS).normalize();
	}
}


void KcAxis::fixTextLayout_(KeTextLayout lay, const size_t& textBox, point3& topLeft, vec3& hDir, vec3& vDir)
{
	switch (lay)
	{
	case KcAxis::k_horz_bottom: // ���µߵ���topLeft���������½�
		topLeft += hDir * textBox.x() + vDir * textBox.y();
		hDir *= -1; vDir *= -1;
		break;

	case KcAxis::k_vert_left: // ���沼�֣�topLeft���������½�
		topLeft += vDir * textBox.y();
		std::swap(hDir, vDir); hDir *= -1;
		break;

	case KcAxis::k_vert_right:// ���沼�֣�topLeft���������Ͻ�
		topLeft += hDir * textBox.x();
		std::swap(hDir, vDir); vDir *= -1;
		break;

	case KcAxis::k_horz_top: // ȱʡ���֣���������
	default:
		break;
	}
}


void KcAxis::fixTextRotation_(const KpTextContext& cxt, const point3& anchor, point3& topLeft, vec3& hDir, vec3& vDir) const
{
	if (cxt.yaw == 0)
		return;

	KtQuaternion<float_t> quat(cxt.yaw, vDir.cross(hDir).normalize());

	auto yaw = quat.yaw();

	hDir = (quat * hDir).normalize();
	vDir = (quat * vDir).normalize();
	//assert(KuMath<float_t>::almostEqual(hDir.dot(vDir), 0));
	topLeft = anchor + quat * (topLeft - anchor);
}


void KcAxis::drawText_(KvPaint* paint, const std::string_view& label, const KpTextContext& cxt, const point3& anchor, bool calcBox) const
{
	point3 topLeft;
	vec3 hDir, vDir;
	calcTextPos_(paint, label.data(), cxt, anchor, topLeft, hDir, vDir);
	assert(KuMath::almostEqual(1.0, hDir.length()));
	assert(KuMath::almostEqual(1.0, vDir.length()));

	if (!calcBox) {
		paint->drawText(topLeft, hDir, vDir, label.data());
	}
	//else {
		auto sz = paint->textSize(label.data());
		auto h = hDir * sz.x() * orientScale_(paint, hDir);
		auto v = vDir * sz.y() * orientScale_(paint, vDir);
		box_.merge({ topLeft, topLeft + h + v });
	//}
}


void KcAxis::calcTitleAnchor_(KvPaint* paint) const
{
	auto center = (realStart() + realEnd()) / 2;

	aabb_t inner(realStart(), realEnd());
	auto low = box_.lower() - inner.lower();
	auto up = box_.upper() - inner.upper();
	auto orient = labelOrient_;
	point3 shift;
	for (int i = 0; i < 3; i++)
		shift[i] = std::max(orient[i] * low[i], orient[i] * up[i]);

	titleAnchor_ = center + orient * shift;

	// ����padding
	titleAnchor_ += orient * titlePadding_ * orientScale_(paint, orient);

	// paint->drawBox(box_.lower(), box_.upper()); // for debug
}


namespace kPrivate
{
	static int otherDim[][2] = {
		{ 1, 2 }, { 0, 2 }, { 0, 1 }
	};
}


KcAxis::point3 KcAxis::realStart() const
{
	point3 st = start();
	if (dimReal_ != -1) {
		for (int i = 0; i < 2; i++)
			st[kPrivate::otherDim[dimReal_][i]] += offset_[i];
	}
	return st;
}


KcAxis::point3 KcAxis::realEnd() const
{
	point3 ed = end();
	if (dimReal_ != -1) {
		for (int i = 0; i < 2; i++)
			ed[kPrivate::otherDim[dimReal_][i]] += offset_[i];
	}
	return ed;
}
