#include "KcCoord2d.h"
#include "KcAxis.h"
#include "KtPoint.h"
#include "KvPaint.h"


KcCoord2d::KcCoord2d()
    : KcCoord2d(point2(0), point2(1))
{

}


KcCoord2d::KcCoord2d(const point2& lower, const point2& upper)
	: KvRenderable("CoordSystem2d")
{
	for (unsigned i = 0; i < 4; i++)
		axes_[i].push_back(std::make_shared<KcAxis>());

	setExtents(lower, upper);

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
}


void KcCoord2d::setExtents(const point2& lower, const point2& upper)
{
	// 只更新4根主坐标轴的range
	KtPoint<float_t, 3> bottomLeft(lower.x(), lower.y(), 0);
	KtPoint<float_t, 3> topRight(upper.x(), upper.y(), 0);
	KtPoint<float_t, 3> bottomRight(upper.x(), lower.y(), 0);
	KtPoint<float_t, 3> topLeft(lower.x(), upper.y(), 0);

	axes_[k_axis_bottom].front()->setRange(lower.x(), upper.x());
	axes_[k_axis_bottom].front()->setExtend(bottomLeft, bottomRight);

	axes_[k_axis_top].front()->setRange(lower.x(), upper.x());
	axes_[k_axis_top].front()->setExtend(topLeft, topRight);

	axes_[k_axis_left].front()->setRange(lower.y(), upper.y());
	axes_[k_axis_left].front()->setExtend(bottomLeft, topLeft);

	axes_[k_axis_right].front()->setRange(lower.y(), upper.y());
	axes_[k_axis_right].front()->setExtend(bottomRight, topRight);
}


KcCoord2d::point2 KcCoord2d::lower() const
{
	return { 
		axes_[k_axis_bottom].front()->lower(), 
		axes_[k_axis_left].front()->lower() 
	};
}


KcCoord2d::point2 KcCoord2d::upper() const
{
	return { 
		axes_[k_axis_bottom].front()->upper(), 
		axes_[k_axis_left].front()->upper() 
	};
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

		for (unsigned i = 0; i < std::size(axes_); i++) {
			auto axislist = axes_[i];
			for (auto axis : axislist)
				if (axis && axis->visible())
					axis->draw(paint);
		}

		paint->viewport() = oldVp; // restore the old viewport
	}
}


KcCoord2d::aabb_type KcCoord2d::boundingBox() const
{
	return { 
		{ 0, 0, 0 }, 
		{ 1, 1, 1 } 
	};
}
