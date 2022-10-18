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
		axes_[i].push_back(std::make_shared<KcAxis>());

	setExtents({ lower.x(), lower.y(), -1 }, { upper.x(), upper.y(), 1 });

	axes_[k_axis_left].front()->tickOrient() =
		axes_[k_axis_left].front()->labelOrient() = -KcAxis::vec3::unitX();
	axes_[k_axis_right].front()->tickOrient() =
		axes_[k_axis_right].front()->labelOrient() = KcAxis::vec3::unitX();
	axes_[k_axis_bottom].front()->tickOrient() =
		axes_[k_axis_bottom].front()->labelOrient() = -KcAxis::vec3::unitY();
	axes_[k_axis_top].front()->tickOrient() =
		axes_[k_axis_top].front()->labelOrient() = KcAxis::vec3::unitY();

	for (unsigned i = 0; i < 4; i++)
		axes_[i].front()->showTick() = true, axes_[i].front()->showLabel() = true;

	axes_[k_axis_right].front()->visible() = false;
	axes_[k_axis_top].front()->visible() = false;

	plane_ = std::make_shared<KcCoordPlane>(axes_[k_axis_left].front(), 
		axes_[k_axis_right].front(), 
		axes_[k_axis_bottom].front(), 
		axes_[k_axis_top].front());
}


void KcCoord2d::setExtents(const point3& lower, const point3& upper)
{
	// 只更新4根主坐标轴的range
	point3 bottomLeft(lower);
	point3 topRight(upper);
	point3 bottomRight(upper.x(), lower.y(), 0);
	point3 topLeft(lower.x(), upper.y(), 0);

	axes_[k_axis_bottom].front()->setRange(lower.x(), upper.x());
	axes_[k_axis_bottom].front()->setExtend(bottomLeft, bottomRight);

	axes_[k_axis_top].front()->setRange(lower.x(), upper.x());
	axes_[k_axis_top].front()->setExtend(topLeft, topRight);

	axes_[k_axis_left].front()->setRange(lower.y(), upper.y());
	axes_[k_axis_left].front()->setExtend(bottomLeft, topLeft);

	axes_[k_axis_right].front()->setRange(lower.y(), upper.y());
	axes_[k_axis_right].front()->setExtend(bottomRight, topRight);
}


KcCoord2d::point3 KcCoord2d::lower() const
{
	return { 
		axes_[k_axis_bottom].front()->lower(), 
		axes_[k_axis_left].front()->lower(),
		-1
	};
}


KcCoord2d::point3 KcCoord2d::upper() const
{
	return { 
		axes_[k_axis_bottom].front()->upper(), 
		axes_[k_axis_left].front()->upper(),
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

		auto l = axes_[k_axis_left].front()->calcMargins(paint);
		auto r = axes_[k_axis_right].front()->calcMargins(paint);
		auto t = axes_[k_axis_top].front()->calcMargins(paint);
		auto b = axes_[k_axis_bottom].front()->calcMargins(paint);

		l.makeCeil(r); l.makeCeil(b); l.makeCeil(t);

		auto oldVp = paint->viewport(); // save the old viewport
		auto newVp = paint->viewport().shrink({ l.left(), l.top() }, { l.right(), l.bottom() });
		paint->setViewport(newVp);

		KvCoord::draw(paint);

		paint->viewport() = oldVp; // restore the old viewport
	}
}
