#include "KcCoord2d.h"
#include "KcAxis.h"
#include "KcCoordPlane.h"
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
		axes_[i].emplace_back(new KcAxis(KcAxis::KeAxisType(i), dim[i], true));

	setExtents({ lower.x(), lower.y(), -1 }, { upper.x(), upper.y(), 1 });

	axes_[KcAxis::k_left].front()->tickOrient() =
		axes_[KcAxis::k_left].front()->labelOrient() = -KcAxis::vec3::unitX();
	axes_[KcAxis::k_right].front()->tickOrient() =
		axes_[KcAxis::k_right].front()->labelOrient() = KcAxis::vec3::unitX();
	axes_[KcAxis::k_bottom].front()->tickOrient() =
		axes_[KcAxis::k_bottom].front()->labelOrient() = -KcAxis::vec3::unitY();
	axes_[KcAxis::k_top].front()->tickOrient() =
		axes_[KcAxis::k_top].front()->labelOrient() = KcAxis::vec3::unitY();

	for (unsigned i = 0; i < 4; i++)
		axes_[i].front()->showTick() = true, axes_[i].front()->showLabel() = true;

	axes_[KcAxis::k_right].front()->visible() = false;
	axes_[KcAxis::k_top].front()->visible() = false;

	// FIXME: 为了兼容3d，此处使用k_back类型，实际应为k_front
	plane_ = std::make_shared<KcCoordPlane>(KcCoordPlane::k_back, axes_[KcAxis::k_left].front(),
		axes_[KcAxis::k_right].front(),
		axes_[KcAxis::k_bottom].front(),
		axes_[KcAxis::k_top].front());

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


void KcCoord2d::setExtents(const point3& lower, const point3& upper)
{
	// 只更新4根主坐标轴的range
	point3 bottomLeft(lower);
	point3 topRight(upper);
	point3 bottomRight(upper.x(), lower.y(), 0);
	point3 topLeft(lower.x(), upper.y(), 0);

	axes_[KcAxis::k_bottom].front()->setRange(lower.x(), upper.x());
	axes_[KcAxis::k_bottom].front()->setExtend(bottomLeft, bottomRight);

	axes_[KcAxis::k_top].front()->setRange(lower.x(), upper.x());
	axes_[KcAxis::k_top].front()->setExtend(topLeft, topRight);

	axes_[KcAxis::k_left].front()->setRange(lower.y(), upper.y());
	axes_[KcAxis::k_left].front()->setExtend(bottomLeft, topLeft);

	axes_[KcAxis::k_right].front()->setRange(lower.y(), upper.y());
	axes_[KcAxis::k_right].front()->setExtend(bottomRight, topRight);
}


KcCoord2d::point3 KcCoord2d::lower() const
{
	return { 
		axes_[KcAxis::k_bottom].front()->lower(), 
		axes_[KcAxis::k_left].front()->lower(),
		-1
	};
}


KcCoord2d::point3 KcCoord2d::upper() const
{
	return { 
		axes_[KcAxis::k_bottom].front()->upper(), 
		axes_[KcAxis::k_left].front()->upper(),
		1
	};
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


void KcCoord2d::draw(KvPaint* paint) const
{
	if (visible()) {
		auto oldVp = paint->viewport();
		paint->setViewport(plane_->innerRect());
		KvCoord::draw(paint);
		paint->setViewport(oldVp); // restore the old viewport
	}
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
	// 重新布局axis
	forAxis([this](KcAxis& axis) {
		KuLayoutHelper::take(&axis);
		return true; });
	
	for (auto iter = axes_[KcAxis::k_left].rbegin();
		iter != axes_[KcAxis::k_left].rend();
		iter++) 
		if ((*iter)->visible() && (*iter)->length() > 0)
			KuLayoutHelper::placeLeft(plane_.get(), iter->get(), 0);

	for (auto iter = axes_[KcAxis::k_right].rbegin();
		iter != axes_[KcAxis::k_right].rend();
		iter++)
		if ((*iter)->visible() && (*iter)->length() > 0)
			KuLayoutHelper::placeRight(plane_.get(), iter->get(), 0);

	for (auto iter = axes_[KcAxis::k_top].rbegin();
		iter != axes_[KcAxis::k_top].rend();
		iter++)
		if ((*iter)->visible() && (*iter)->length() > 0)
			KuLayoutHelper::placeTop(plane_.get(), iter->get(), 0);

	for (auto iter = axes_[KcAxis::k_bottom].rbegin();
		iter != axes_[KcAxis::k_bottom].rend();
		iter++)
		if ((*iter)->visible() && (*iter)->length() > 0)
			KuLayoutHelper::placeBottom(plane_.get(), iter->get(), 0);


	return __super::calcSize_(cxt);
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
