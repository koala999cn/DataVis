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

	axes_[k_left].front()->tickOrient() = 
		axes_[k_left].front()->labelOrient() = -KcAxis::vec3::unitX();
	axes_[k_right].front()->tickOrient() =
		axes_[k_right].front()->labelOrient() = KcAxis::vec3::unitX();
	axes_[k_bottom].front()->tickOrient() =
		axes_[k_bottom].front()->labelOrient() = -KcAxis::vec3::unitY();
	axes_[k_top].front()->tickOrient() =
		axes_[k_top].front()->labelOrient() = KcAxis::vec3::unitY();

	//axes_[k_left].front()->tickOrient() = KcAxis::vec3::unitX();
	//axes_[k_right].front()->tickOrient() = -KcAxis::vec3::unitX();
	//axes_[k_bottom].front()->tickOrient() = KcAxis::vec3::unitY();
	//axes_[k_top].front()->tickOrient() = -KcAxis::vec3::unitY();

	for (unsigned i = 0; i < 4; i++)
		axes_[i].front()->showTick() = true, axes_[i].front()->showLabel() = true;

	//axes_[k_right].front()->visible() = false;
	//axes_[k_top].front()->visible() = false;
}


void KcCoord2d::setExtents(const point2& lower, const point2& upper)
{
	// 只更新4根主坐标轴的range
	KtPoint<float_t, 3> bottomLeft(lower.x(), lower.y(), 0);
	KtPoint<float_t, 3> topRight(upper.x(), upper.y(), 0);
	KtPoint<float_t, 3> bottomRight(upper.x(), lower.y(), 0);
	KtPoint<float_t, 3> topLeft(lower.x(), upper.y(), 0);

	axes_[k_bottom].front()->setRange(lower.x(), upper.x());
	axes_[k_bottom].front()->setStart(bottomLeft);
	axes_[k_bottom].front()->setEnd(bottomRight);

	axes_[k_top].front()->setRange(lower.x(), upper.x());
	axes_[k_top].front()->setStart(topLeft);
	axes_[k_top].front()->setEnd(topRight);

	axes_[k_left].front()->setRange(lower.y(), upper.y());
	axes_[k_left].front()->setStart(bottomLeft);
	axes_[k_left].front()->setEnd(topLeft);

	axes_[k_right].front()->setRange(lower.y(), upper.y());
	axes_[k_right].front()->setStart(bottomRight);
	axes_[k_right].front()->setEnd(topRight);
}


KcCoord2d::point2 KcCoord2d::lower() const
{
	return { axes_[k_bottom].front()->lower(), axes_[k_left].front()->lower() };
}


KcCoord2d::point2 KcCoord2d::upper() const
{
	return { axes_[k_bottom].front()->upper(), axes_[k_left].front()->upper() };
}


void KcCoord2d::draw(KvPaint* paint) const
{
	if (visible()) {

		auto l = axes_[KcCoord2d::k_left].front()->calcMargins(paint);
		auto r = axes_[KcCoord2d::k_right].front()->calcMargins(paint);
		auto t = axes_[KcCoord2d::k_top].front()->calcMargins(paint);
		auto b = axes_[KcCoord2d::k_bottom].front()->calcMargins(paint);

		l = l.ceil(l, r);
		t = t.ceil(t, b);
		l = l.ceil(l, t);

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
	return { { 0, 0, 0 }, { 1, 1, 1 } };
}
