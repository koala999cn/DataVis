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

	axes_[k_left].front()->setTickOrient(KcAxis::k_neg_x);
	axes_[k_right].front()->setTickOrient(KcAxis::k_x);
	axes_[k_bottom].front()->setTickOrient(KcAxis::k_neg_y);
	axes_[k_top].front()->setTickOrient(KcAxis::k_y);

	for (unsigned i = 0; i < 4; i++)
		axes_[i].front()->setShowTick(true), axes_[i].front()->setShowLabel(true);

	axes_[k_right].front()->visible() = false;
	axes_[k_top].front()->visible() = false;
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

		auto l = axes_[KcCoord2d::k_left].front()->calcSize(paint).x();
		auto r = axes_[KcCoord2d::k_right].front()->calcSize(paint).x();
		auto t = axes_[KcCoord2d::k_top].front()->calcSize(paint).x();
		auto b = axes_[KcCoord2d::k_bottom].front()->calcSize(paint).x();

		auto vp = paint->viewport(); // save the old viewport
		paint->setViewport(paint->viewport().shrink({ l, t }, { r, b }));

		for (unsigned i = 0; i < std::size(axes_); i++) {
			auto axislist = axes_[i];
			for (auto axis : axislist)
				if (axis && axis->visible())
					axis->draw(paint);
		}

		paint->viewport() = vp; // restore the old viewport
	}
}


KcCoord2d::aabb_type KcCoord2d::boundingBox() const
{
	return { {0, 0, 0}, {1, 1, 1} };
}
