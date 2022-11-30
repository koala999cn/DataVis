#include "KcCoord2d.h"
#include "KcAxis.h"
#include "KcCoordPlane.h"
#include "KvPaint.h"


KcCoord2d::KcCoord2d()
    : KcCoord2d(point2(0), point2(1))
{
	
}


KcCoord2d::KcCoord2d(const point2& lower, const point2& upper)
	: KvCoord("CoordSystem2d")
{
	for (unsigned i = 0; i < 4; i++)
		axes_[i].push_back(std::make_shared<KcAxis>(KcAxis::KeAxisType(i)));

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

	// FIXME: Ϊ�˼���3d���˴�ʹ��k_back���ͣ�ʵ��ӦΪk_front
	plane_ = std::make_shared<KcCoordPlane>(KcCoordPlane::k_back, axes_[KcAxis::k_left].front(),
		axes_[KcAxis::k_right].front(),
		axes_[KcAxis::k_bottom].front(),
		axes_[KcAxis::k_top].front());

	layMgr_.setRoot(plane_.get());
}


KcCoord2d::~KcCoord2d()
{
	// ���layMgr_�е�plane��axis���󣬷�ֹ����release
	forPlane([this](KcCoordPlane& plan) {
		layMgr_.take(&plan);
		return true; });
	forAxis([this](KcAxis& axis) {
		layMgr_.take(&axis);
		return true; });
}


void KcCoord2d::setExtents(const point3& lower, const point3& upper)
{
	// ֻ����4�����������range
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


KtMargins<KcCoord2d::float_t> KcCoord2d::calcMargins(KvPaint* paint) const
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
	// ���²���axis
	forAxis([this](KcAxis& axis) {
		layMgr_.take(&axis);
		return true; });
	
	for (auto iter = axes_[KcAxis::k_left].rbegin();
		iter != axes_[KcAxis::k_left].rend();
		iter++) 
		if ((*iter)->visible() && (*iter)->length() > 0)
		    layMgr_.placeLeft(plane_.get(), iter->get(), 0);

	for (auto iter = axes_[KcAxis::k_right].rbegin();
		iter != axes_[KcAxis::k_right].rend();
		iter++)
		if ((*iter)->visible() && (*iter)->length() > 0)
		    layMgr_.placeRight(plane_.get(), iter->get(), 0);

	for (auto iter = axes_[KcAxis::k_top].rbegin();
		iter != axes_[KcAxis::k_top].rend();
		iter++)
		if ((*iter)->visible() && (*iter)->length() > 0)
		    layMgr_.placeTop(plane_.get(), iter->get(), 0);

	for (auto iter = axes_[KcAxis::k_bottom].rbegin();
		iter != axes_[KcAxis::k_bottom].rend();
		iter++)
		if ((*iter)->visible() && (*iter)->length() > 0)
		    layMgr_.placeBottom(plane_.get(), iter->get(), 0);


	layMgr_.root()->calcSize(cxt);
	return { layMgr_.root()->innerRect().width(),
		layMgr_.root()->innerRect().height() };
}


void KcCoord2d::arrange(const rect_t& rc)
{
	__super::arrange(rc);
	layMgr_.root()->arrange(rc);
}


point2i KcCoord2d::extraShares() const
{
	return layMgr_.root()->extraShares() * shareFactor();
}


KcCoord2d::rect_t KcCoord2d::getPlotRect() const
{
	return plane_->innerRect();
}
