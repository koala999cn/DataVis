#include "KcAxis.h"
#include <assert.h>
#include "KcLinearTicker.h"
#include "KvPaint.h"
#include "KtuMath.h"
#include "KtLine.h"
#include "KtQuaternion.h"
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

	// NB: 无论calcBox是否为true，都须重新计算box_
	// 因为计算布局（calcBox为true）和真实绘制（calcBox为false）时，
	// 变化矩阵堆栈可能不同，特别是后者可能新压入了scale矩阵，
	// 这就到这前期计算的box_和其他与世界坐标相关的长度和位置不可用
	box_.setExtents(point3(0), point3(0));

	// draw baseline
	if (showBaseline() && baselineCxt_.style != KpPen::k_none) {
		if (!calcBox) {
			paint->apply(baselineCxt_);
			paint->drawLine(start(), end()); // 物理坐标
		}

		box_ = aabb_t(start(), end());
	}

	auto realShowTitle = showTitle() && !title().empty();

	// tickOrient_和labelOrient_只计算一次
	if (realShowTitle || showTick() || showLabel()) {
		//if (calcBox) {
			calcTickOrient_(paint);
			calcLabelOrient_(paint);
		//}
	}


	// draw ticks & label
	if (showTick() || showLabel())
		drawTicks_(paint, calcBox);

	// draw title
	if (realShowTitle) {
		
		if (!calcBox)
			paint->setColor(titleContext().color);

		calcTitleAnchor_(paint); 
		drawText_(paint, title_, titleCxt_, titleAnchor_, calcBox);
	}
}


KcAxis::vec3 KcAxis::outsideOrient_(KvPaint* paint) const
{
	// 12根坐标轴的默认外向朝向
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

	auto o = outsideOrient[typeReal()];
	if (paint->currentCoord() == KvPaint::k_coord_screen ||
		paint->currentCoord() == KvPaint::k_coord_local_screen)
		o.y() *= -1; // TODO: 有无更好的方法

	return o;
}


KcAxis::vec3 KcAxis::insideOrient_(KvPaint* paint) const
{
	return -outsideOrient_(paint);
}


KcAxis::vec3 KcAxis::axisOrient_() const
{
	return (end() - start()).getNormalize();
}


void KcAxis::calcTickOrient_(KvPaint* paint) const
{
	// 此处的orient为世界坐标
	vec3 tickOrient = (tickCxt_.side == k_inside) ? insideOrient_(paint) : outsideOrient_(paint);

	auto vAxis = paint->localToWorldV(axisOrient_()).normalize(); // 坐标轴方向矢量
	if (tickCxt_.pitch != 0) {
		KtQuaternion<float_t> quatPitch(tickCxt_.pitch, vAxis); // 绕坐标轴旋转pitch弧度
		tickOrient = quatPitch * tickOrient;
	}
	
	if (tickCxt_.yaw != 0) {
		auto vPrep = tickOrient.cross(vAxis).normalize(); // 刻度线和坐标轴的垂直矢量
		KtQuaternion<float_t> quatYaw(tickCxt_.yaw, vPrep);
		tickOrient = quatYaw * tickOrient;
	}

	tickOrient_ = paint->worldToLocalV(tickOrient).normalize(); // 变换回局部坐标系
}


void KcAxis::calcLabelOrient_(KvPaint* paint) const
{
	labelOrient_ = tickCxt_.side == k_inside ? -tickOrient_ : tickOrient_;

	// 将labelOrient_变换为垂直于axis的方向
	auto axisOrient = axisOrient_();
	labelOrient_ = axisOrient.cross(labelOrient_);
	labelOrient_ = labelOrient_.cross(axisOrient).getNormalize();

	// NB:确保label始终朝外侧（当tickOrient与axisOrient平行时，labelOrient_可能指向内侧）
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
	assert(KtuMath<float_t>::almostEqual(o.length(), 1.0));
	return 1. / paint->projectv(o).length();
}


void KcAxis::drawTicks_(KvPaint* paint, bool calcBox) const
{
	assert(showTick() || showLabel());

	if (length() == 0)
		return; // TODO: draw or not draw ? draw what ??

	ticker()->generate(lower(), upper(), showSubtick(), showLabel());
	const auto& ticks = ticker()->ticks();


	// 计算屏幕坐标1个像素尺度，相当于世界坐标多少个单位长度
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
	auto axisOrient = paint->projectv(end() - start());
	auto labelOrient = paint->projectv(labelOrient_); // TODO: 区分label和title

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
	assert(visible());

	if (length() > 0) {

		auto paint = (KvPaint*)cxt;
		auto marg = calcMargins(paint);

		switch (typeReal())
		{
		case KcAxis::k_left:
			return { std::max<float_t>(marg.left(), baselineCxt_.width), 0 };

		case KcAxis::k_right:
			return { std::max<float_t>(marg.right(), baselineCxt_.width), 0 };

		case KcAxis::k_bottom:
			return { 0, std::max<float_t>(marg.bottom(), baselineCxt_.width) };

		case KcAxis::k_top:
			return { 0, std::max<float_t>(marg.top(), baselineCxt_.width) };

		default:
			break;
		}
	}

	return { 0, 0 };
}


// calcMargins主要进行虚拟绘制，以计算以像素为单位的margins为主
// draw则在世界坐标系进行实际绘制，paint在执行绘制指令时负责坐标转换
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
	//assert(margs.ge({ 0, 0, 0, 0 }));

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

	// 根据dimReal_和dimSwapped_获取swapKind
	// 使用[dimReal_][dimSwapped_ + 1]索引
	constexpr static int swapKind[3][4] = {
		{ swap_yz, swap_none, swap_xy, swap_xz },
		{ swap_xz, swap_xy, swap_none, swap_yz },
		{ swap_xy, swap_xz, swap_yz, swap_none }
	};
	
	// NB：另外两个维度的坐标轴交换，也会影响当前坐标轴的方位。目前尚未实现交换情况下的方位一致性
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
	
	if (dimReal_ == -1) return type_; // color-bar的坐标轴，无交换

	auto t = swapType[type_][swapKind[dimReal_][dimSwapped_ + 1]];
	return KeType(t);
}


void KcAxis::calcTextPos_(KvPaint* paint, const std::string_view& label, const KpTextContext& cxt, 
	const point3& anchor, point3& topLeft, vec3& hDir, vec3& vDir) const
{
	auto textBox = paint->textSize(label.data());
	if (cxt.layout == k_vert_left || cxt.layout == k_vert_right)
		std::swap(textBox.x(), textBox.y());
	
	if (cxt.billboard) { // 公告牌模式，文字始终顺着+x轴延展

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
			hDir *= -1; // 修正hDir，确保文字在三维空间的可读性

		topLeft = anchor - hDir * (textBox.x() / 2) * orientScale_(paint, hDir);
	}

	// fixTextLayout_需要textBox为世界坐标尺寸，此处进行变换
	textBox *= point2(orientScale_(paint, hDir), orientScale_(paint, vDir));
	fixTextLayout_(cxt.layout, textBox, topLeft, hDir, vDir);

	fixTextRotation_(cxt, anchor, topLeft, hDir, vDir);

	if (cxt.billboard && cxt.yaw) {
		// 调整vDir，确保与hDir在屏幕坐标系下垂直
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
	case KcAxis::k_horz_bottom: // 上下颠倒，topLeft调整到右下角
		topLeft += hDir * textBox.x() + vDir * textBox.y();
		hDir *= -1; vDir *= -1;
		break;

	case KcAxis::k_vert_left: // 竖版布局，topLeft调整到左下角
		topLeft += vDir * textBox.y();
		std::swap(hDir, vDir); hDir *= -1;
		break;

	case KcAxis::k_vert_right:// 竖版布局，topLeft调整到右上角
		topLeft += hDir * textBox.x();
		std::swap(hDir, vDir); vDir *= -1;
		break;

	case KcAxis::k_horz_top: // 缺省布局，不作调整
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
	//assert(KtuMath<float_t>::almostEqual(hDir.dot(vDir), 0));
	topLeft = anchor + quat * (topLeft - anchor);
}


void KcAxis::drawText_(KvPaint* paint, const std::string_view& label, const KpTextContext& cxt, const point3& anchor, bool calcBox) const
{
	point3 topLeft;
	vec3 hDir, vDir;
	calcTextPos_(paint, label.data(), cxt, anchor, topLeft, hDir, vDir);
	assert(KtuMath<float_t>::almostEqual(1.0, hDir.length()));
	assert(KtuMath<float_t>::almostEqual(1.0, vDir.length()));

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
	auto center = (start() + end()) / 2;

	aabb_t inner(start(), end());
	auto low = box_.lower() - inner.lower();
	auto up = box_.upper() - inner.upper();
	auto orient = labelOrient_;
	point3 shift;
	for (int i = 0; i < 3; i++)
		shift[i] = std::max(orient[i] * low[i], orient[i] * up[i]);

	titleAnchor_ = center + orient * shift;

	// 加上padding
	titleAnchor_ += orient * titlePadding_ * orientScale_(paint, orient);

	// paint->drawBox(box_.lower(), box_.upper()); // for debug
}
