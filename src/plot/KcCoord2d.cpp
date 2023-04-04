#include "KcCoord2d.h"
#include "KcAxis.h"
#include "KcCoordPlane.h"
#include "KvPlottable.h"
#include "KvPaint.h"
#include "layout/KuLayoutHelper.h"


KcCoord2d::KcCoord2d()
    : KcCoord2d(point2(0), point2(1))
{
	
}


KcCoord2d::KcCoord2d(const point2& lower, const point2& upper)
	: KvCoord("CoordSystem2d")
{
	int dim[4];
	dim[KcAxis::k_top] = dim[KcAxis::k_bottom] = 0;
	dim[KcAxis::k_left] = dim[KcAxis::k_right] = 1;

	// 初始化4根主坐标轴
	for (unsigned i = 0; i < 4; i++) 
		axes_[i].emplace_back(new KcAxis(KcAxis::KeType(i), dim[i], true));

	setExtents({ lower.x(), lower.y(), -1 }, { upper.x(), upper.y(), 1 });

	static const char* title[] = { "X", "Y" };
	for (unsigned i = 0; i < 4; i++)
		axes_[i].front()->title() = title[axes_[i].front()->dim()];

	axes_[KcAxis::k_right].front()->visible() = false;
	axes_[KcAxis::k_top].front()->visible() = false;

	// FIXME: 为了兼容3d，此处使用k_back类型，实际应为k_front
	plane_ = std::make_shared<KcCoordPlane>(KcCoordPlane::k_back,
		axes_[KcAxis::k_bottom].front(),
		axes_[KcAxis::k_top].front(),
		axes_[KcAxis::k_left].front(),
		axes_[KcAxis::k_right].front());

	putAt(0, 0, plane_.get());
}


KcCoord2d::~KcCoord2d()
{
	// 清除layout中的plane和axis对象，防止二次release
	forPlane([this](KcCoordPlane& plan) {
		KuLayoutHelper::take(&plan);
		return true; });
	forAxis([this](KcAxis& axis) {
		KuLayoutHelper::take(&axis);
		return true; });
}


void KcCoord2d::forAxis(std::function<bool(KcAxis& axis)> fn) const
{
	for (unsigned i = 0; i < std::size(axes_); i++)
		for(auto& aixs : axes_[i])
		    if(!fn(*aixs))
				return;
}


void KcCoord2d::forPlane(std::function<bool(KcCoordPlane& plane)> fn) const
{
	fn(*plane_);
}


std::shared_ptr<KcAxis> KcCoord2d::defaultAxis(unsigned dim) const
{
	static unsigned dimMap[] = { KcAxis::k_bottom, KcAxis::k_left };
	return axes_[dimMap[dim]].front();
}


KtMargins<KcCoord2d::float_t> KcCoord2d::calcMargins_(KvPaint* paint) const
{
	auto l = axes_[KcAxis::k_left].front()->calcMargins(paint);
	auto r = axes_[KcAxis::k_right].front()->calcMargins(paint);
	auto t = axes_[KcAxis::k_top].front()->calcMargins(paint);
	auto b = axes_[KcAxis::k_bottom].front()->calcMargins(paint);

	l.makeCeil(r); l.makeCeil(b); l.makeCeil(t);

	return l;
}


KcCoord2d::size_t KcCoord2d::calcSize_(void* cxt) const
{
	const_cast<KcCoord2d*>(this)->margins() = { point2(0), point2(0) }; // TOOD: 是否可推迟到fixMargins_

	// 重新布局axis
	forAxis([this](KcAxis& axis) {
		KuLayoutHelper::take(&axis);
		return true; });
	
	forAxis([this](KcAxis& axis) {
		if (axis.visible() && axis.length() > 0) {
			switch (axis.typeReal())
			{
			case KcAxis::k_left:
				KuLayoutHelper::placeLeft(plane_.get(), &axis, axis.main() ? 0 : -1);
				break;

			case KcAxis::k_right:
				KuLayoutHelper::placeRight(plane_.get(), &axis, axis.main() ? 0 : -1);
				break;

			case KcAxis::k_top:
				KuLayoutHelper::placeTop(plane_.get(), &axis, axis.main() ? 0 : -1);
				break;

			case KcAxis::k_bottom:
				KuLayoutHelper::placeBottom(plane_.get(), &axis, axis.main() ? 0 : -1);
				break;

			default:
				break;
			}
		}
		return true; });

	auto sz = __super::calcSize_(cxt);

	// 同步coord的margins，避免坐标轴的label被裁剪
	const_cast<KcCoord2d*>(this)->fixMargins_();

	return sz;
}


void KcCoord2d::fixMargins_()
{
	// plane左侧和右侧所有元素的累计尺寸
	double sleft(0), sright(0);

	// plane上侧和下侧所有元素的累计尺寸
	double stop(0), sbottom(0);

	double mleft(0), mright(0); // 与palne同列元素的最大左、右留白
	double mtop(0), mbottom(0); // 与plane同行元素的最大上、下留白


	// 遍历左侧
	auto pos = find(plane_.get()); // 定位坐标平面的位置
	for (unsigned i = 0; i < pos.second; i++) {
		auto e = getAt(pos.first, i);
		if (e) {
			sleft += e->expectRoom()[0];
			mtop = std::max(mtop, e->margins().lower()[1]);
			mbottom = std::max(mbottom, e->margins().upper()[1]);
		}
	}

	// 遍历右侧
	for (unsigned i = pos.second + 1; i < cols(); i++) {
		auto e = getAt(pos.first, i);
		if (e) {
			sright += e->expectRoom()[0];
			mtop = std::max(mtop, e->margins().lower()[1]);
			mbottom = std::max(mbottom, e->margins().upper()[1]);
		}
	}

	// 遍历上侧
	for (unsigned i = 0; i < pos.first; i++) {
		auto e = getAt(i, pos.second);
		if (e) {
			stop += e->expectRoom()[1];
			mleft = std::max(mleft, e->margins().lower()[0]);
			mright = std::max(mright, e->margins().upper()[0]);
		}
	}

	// 遍历下侧
	for (unsigned i = pos.first + 1; i < rows(); i++) {
		auto e = getAt(i, pos.second);
		if (e) {
			sbottom += e->expectRoom()[1];
			mleft = std::max(mleft, e->margins().lower()[0]);
			mright = std::max(mright, e->margins().upper()[0]);
		}
	}

	// 左侧空间不足，拓展左侧留白
	if (mleft > sleft) 
		margins().lower().x() = (mleft - sleft);

	// 右侧空间不足，拓展右侧留白
	if (mright > sright) 
		margins().upper().x() = (mright - sright);

	// 下侧空间不足，拓展下侧留白
	if (mbottom > sbottom) 
		margins().upper().y() = (mbottom - sbottom);

	// 上侧空间不足，拓展上侧留白
	if (mtop > stop)
		margins().lower().y() = (mtop - stop);
}


KcCoord2d::rect_t KcCoord2d::getPlotRect() const
{
	return plane_->innerRect();
}


void KcCoord2d::placeElement(KvLayoutElement* ele, KeAlignment loc)
{
	assert(!isAncestorOf(ele));

	if (loc.inner()) {
		plane_->insert(ele);
	}
	else {
		if (loc & KeAlignment::k_horz_first) {
			if (loc & KeAlignment::k_left)
				KuLayoutHelper::placeLeft(plane_.get(), ele, -1);
			else if (loc & KeAlignment::k_right)
				KuLayoutHelper::placeRight(plane_.get(), ele, -1);
			else if (loc & KeAlignment::k_top)
				KuLayoutHelper::placeTop(plane_.get(), ele, -1);
			else if (loc & KeAlignment::k_bottom)
				KuLayoutHelper::placeBottom(plane_.get(), ele, -1);
			else
				assert(false);
		}
		else {
			if (loc & KeAlignment::k_top)
				KuLayoutHelper::placeTop(plane_.get(), ele, -1);
			else if (loc & KeAlignment::k_bottom)
				KuLayoutHelper::placeBottom(plane_.get(), ele, -1);
			else if (loc & KeAlignment::k_left)
				KuLayoutHelper::placeLeft(plane_.get(), ele, -1);
			else if (loc & KeAlignment::k_right)
				KuLayoutHelper::placeRight(plane_.get(), ele, -1);
			else
				assert(false);
		}
	}
}


void KcCoord2d::splitAxis(KvPlottable* plt, unsigned dim, int mode)
{
	static const KcAxis::KeType type[][2] = {
		{ KcAxis::k_bottom, KcAxis::k_top },
		{ KcAxis::k_left, KcAxis::k_right }
	};

	if (mode == 0) {
		if (!plt->axis(dim)->main())
			eraseSplitAxis_(plt->axis(dim));
		plt->setAxis(dim, defaultAxis(dim));
	}
	else {
		if (plt->axis(dim)->main()) {
			auto axis = std::make_shared<KcAxis>(*plt->axis(dim));
			axis->setMain(false);
			axis->setType(type[dim][mode-1]);
			axis->visible() = true;
			plt->setAxis(dim, axis);
			addSplitAxis_(axis);
		}
		else {
			eraseSplitAxis_(plt->axis(dim));
			plt->axis(dim)->setType(type[dim][mode - 1]);
			addSplitAxis_(plt->axis(dim));
		}
	}
}


void KcCoord2d::addSplitAxis_(const axis_ptr& axis)
{
	axis->setParent(nullptr);
	axes_[axis->type()].push_back(axis);
}


void KcCoord2d::eraseSplitAxis_(const axis_ptr& axis)
{
	KuLayoutHelper::take(axis.get());

	auto type = axis->type();
	auto pos = std::find(axes_[type].cbegin(), axes_[type].cend(), axis);
	axes_[type].erase(pos);
}
