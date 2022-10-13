#include "KcCoord3d.h"
#include "KcAxis.h"
#include "KcGridPlane.h"
#include "KvPaint.h"
#include <assert.h>


KcCoord3d::KcCoord3d()
	: KcCoord3d(point3(0), point3(1))
{

}


KcCoord3d::KcCoord3d(const point3& lower, const point3& upper)
	: KvRenderable("CoordSystem3d")
{
	// 初始化12根坐标轴
	for (unsigned i = 0; i < std::size(axes_); i++)
		axes_[i].reset(new KcAxis);

	setExtents(lower, upper);

	axes_[k_x0]->setTickOrient(KcAxis::k_neg_y);
	axes_[k_x1]->setTickOrient(KcAxis::k_y);
	axes_[k_x2]->setTickOrient(KcAxis::k_y);
	axes_[k_x3]->setTickOrient(KcAxis::k_neg_y);

	axes_[k_y0]->setTickOrient(KcAxis::k_neg_x);
	axes_[k_y1]->setTickOrient(KcAxis::k_x);
	axes_[k_y2]->setTickOrient(KcAxis::k_x);
	axes_[k_y3]->setTickOrient(KcAxis::k_neg_x);

	axes_[k_z0]->setTickOrient(KcAxis::k_neg_x);
	axes_[k_z1]->setTickOrient(KcAxis::k_x);
	axes_[k_z2]->setTickOrient(KcAxis::k_x);
	axes_[k_z3]->setTickOrient(KcAxis::k_neg_x);

	for(unsigned i = 0; i < std::size(axes_); i++)
	    axes_[i]->setShowTick(false), axes_[i]->setShowLabel(false);

	axes_[k_x3]->setShowTick(true), axes_[k_x3]->setShowLabel(true);
	axes_[k_y3]->setShowTick(true), axes_[k_y3]->setShowLabel(true);
	axes_[k_z1]->setShowTick(true), axes_[k_z1]->setShowLabel(true);


	// 初始化6个grid平面
	planes_[k_xy_back] = std::make_shared<KcGridPlane>(
		axes_[k_x0], axes_[k_x1], axes_[k_y0], axes_[k_y1]);
	planes_[k_xy_front] = std::make_shared<KcGridPlane>(
		axes_[k_x2], axes_[k_x3], axes_[k_y2], axes_[k_y3]);

	planes_[k_yz_left] = std::make_shared<KcGridPlane>(
		axes_[k_y0], axes_[k_y3], axes_[k_z0], axes_[k_z3]);
	planes_[k_yz_right] = std::make_shared<KcGridPlane>(
		axes_[k_y1], axes_[k_y2], axes_[k_z1], axes_[k_z2]);

	planes_[k_xz_ceil] = std::make_shared<KcGridPlane>(
		axes_[k_x1], axes_[k_x2], axes_[k_z2], axes_[k_z3]);
	planes_[k_xz_floor] = std::make_shared<KcGridPlane>(
		axes_[k_x0], axes_[k_x3], axes_[k_z0], axes_[k_z1]);

	for (unsigned i = 0; i < std::size(planes_); i++) {
		planes_[i]->visible() = false;
		planes_[i]->minorVisible() = false;
	}

	planes_[k_xy_back]->visible() = true;
	planes_[k_yz_left]->visible() = true;
	planes_[k_xz_floor]->visible() = true;
}


/*                  
 *    p1 -------x1------  p6
 *     /z3           z2/|
 *  p2 --------x2-----  |y1
 *    |y3           y2| | p5
 *    |_______________|/z1
 *  p3         x3    p4
 *    
 */
void KcCoord3d::setExtents(const point3& lower, const point3& upper)
{
	// p0 = lower, p7 = upper
	auto p1 = point3{ lower.x(), upper.y(), lower.z() };
	auto p2 = point3{ lower.x(), upper.y(), upper.z() };
	auto p3 = point3{ lower.x(), lower.y(), upper.z() };
	auto p4 = point3{ upper.x(), lower.y(), upper.z() };
	auto p5 = point3{ upper.x(), lower.y(), lower.z() };
	auto p6 = point3{ upper.x(), upper.y(), lower.z() };

	axes_[k_x0]->setRange(lower.x(), upper.x());
	axes_[k_x0]->setStart(lower); axes_[k_x0]->setEnd(p5);
	axes_[k_x1]->setRange(lower.x(), upper.x());
	axes_[k_x1]->setStart(p1); axes_[k_x1]->setEnd(p6);
	axes_[k_x2]->setRange(lower.x(), upper.x());
	axes_[k_x2]->setStart(p2); axes_[k_x2]->setEnd(upper);
	axes_[k_x3]->setRange(lower.x(), upper.x());
	axes_[k_x3]->setStart(p3); axes_[k_x3]->setEnd(p4);

	axes_[k_y0]->setRange(lower.y(), upper.y());
	axes_[k_y0]->setStart(lower); axes_[k_y0]->setEnd(p1);
	axes_[k_y1]->setRange(lower.y(), upper.y());
	axes_[k_y1]->setStart(p5); axes_[k_y1]->setEnd(p6);
	axes_[k_y2]->setRange(lower.y(), upper.y());
	axes_[k_y2]->setStart(p4); axes_[k_y2]->setEnd(upper);
	axes_[k_y3]->setRange(lower.y(), upper.y());
	axes_[k_y3]->setStart(p3); axes_[k_y3]->setEnd(p2);

	axes_[k_z0]->setRange(lower.z(), upper.z());
	axes_[k_z0]->setStart(lower); axes_[k_z0]->setEnd(p3);
	axes_[k_z1]->setRange(lower.z(), upper.z());
	axes_[k_z1]->setStart(p5); axes_[k_z1]->setEnd(p4);
	axes_[k_z2]->setRange(lower.z(), upper.z());
	axes_[k_z2]->setStart(p6); axes_[k_z2]->setEnd(upper);
	axes_[k_z3]->setRange(lower.z(), upper.z());
	axes_[k_z3]->setStart(p1); axes_[k_z3]->setEnd(p2);
}


KcCoord3d::point3 KcCoord3d::lower() const
{
	return { axes_[k_x0]->lower(), axes_[k_y0]->lower(), axes_[k_z0]->lower() };
}


KcCoord3d::point3 KcCoord3d::upper() const
{
	return { axes_[k_x0]->upper(), axes_[k_y0]->upper(), axes_[k_z0]->upper() };
}


KcCoord3d::aabb_type KcCoord3d::boundingBox() const
{
	return { lower(), upper() };
}


KcCoord3d::point3 KcCoord3d::center() const
{
	return (lower() + upper()) / 2;
}


double KcCoord3d::diag() const
{
	return (upper() - lower()).abs();
}


void KcCoord3d::zoom(double factor)
{
	auto c = center();
	auto delta = (upper() - lower()) * factor * 0.5;
	setExtents(c - delta, c + delta);

	assert(c.isApproxEqual(center()));
}


void KcCoord3d::draw(KvPaint* paint) const
{
	if (visible()) {
		for (unsigned i = 0; i < std::size(axes_); i++) {
			auto axis = axes_[i];
			auto rlen = diag();
			if (axis && axis->visible()) {
				axis->setRefLength(rlen);
				axis->draw(paint);
			}
		}

		for (unsigned i = 0; i < std::size(planes_); i++) {
			auto& plane = planes_[i];
			if (plane && plane->visible()) {
				plane->draw(paint);
			}
		}
	}
}

