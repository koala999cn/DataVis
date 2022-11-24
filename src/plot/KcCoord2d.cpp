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

	// FIXME: 为了兼容3d，此处使用k_back类型，实际应为k_front
	plane_ = std::make_shared<KcCoordPlane>(KcCoordPlane::k_back, axes_[KcAxis::k_left].front(),
		axes_[KcAxis::k_right].front(),
		axes_[KcAxis::k_bottom].front(),
		axes_[KcAxis::k_top].front());
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





		KvCoord::draw(paint);

		//paint->setViewport(oldVp); // restore the old viewport
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
