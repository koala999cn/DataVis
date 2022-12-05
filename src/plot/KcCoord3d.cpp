#include "KcCoord3d.h"
#include "KcAxis.h"
#include "KcCoordPlane.h"
#include "KvPaint.h"
#include <assert.h>


namespace kPrivate
{
	class KcDummyElement_ : public KvLayoutElement
	{
	private:
		size_t calcSize_(void* cxt) const final { return { 0, 0 }; }
	};
}


KcCoord3d::KcCoord3d()
	: KcCoord3d(point3(0), point3(1))
{
	putAt(0, 0, new kPrivate::KcDummyElement_); // 此处压入一个dummy元素，否则零元素的grid布局尺寸始终为0
}


KcCoord3d::KcCoord3d(const point3& lower, const point3& upper)
	: KvCoord("CoordSystem3d")
{
	// 初始化12根坐标轴
	for (unsigned i = 0; i < std::size(axes_); i++)
		axes_[i].reset(new KcAxis(KcAxis::KeAxisType(i)));

	setExtents(lower, upper);

	axes_[KcAxis::k_far_bottom]->tickOrient() = 
		axes_[KcAxis::k_far_bottom]->labelOrient() = -KcAxis::vec3::unitY();
	axes_[KcAxis::k_far_top]->tickOrient() = 
		axes_[KcAxis::k_far_top]->labelOrient() = KcAxis::vec3::unitY();
	axes_[KcAxis::k_near_top]->tickOrient() = 
		axes_[KcAxis::k_near_top]->labelOrient() = KcAxis::vec3::unitY();
	axes_[KcAxis::k_near_bottom]->tickOrient() = 
		axes_[KcAxis::k_near_bottom]->labelOrient() = -KcAxis::vec3::unitY();

	axes_[KcAxis::k_far_left]->tickOrient() = 
		axes_[KcAxis::k_far_left]->labelOrient() = -KcAxis::vec3::unitX();
	axes_[KcAxis::k_far_right]->tickOrient() = 
		axes_[KcAxis::k_far_right]->labelOrient() = KcAxis::vec3::unitX();
	axes_[KcAxis::k_near_right]->tickOrient() = 
		axes_[KcAxis::k_near_right]->labelOrient() = KcAxis::vec3::unitX();
	axes_[KcAxis::k_near_left]->tickOrient() = 
		axes_[KcAxis::k_near_left]->labelOrient() = -KcAxis::vec3::unitX();

	axes_[KcAxis::k_floor_left]->tickOrient() =
		axes_[KcAxis::k_floor_left]->labelOrient() = -KcAxis::vec3::unitX();
	axes_[KcAxis::k_floor_right]->tickOrient() = 
		axes_[KcAxis::k_floor_right]->labelOrient() = KcAxis::vec3::unitX();
	axes_[KcAxis::k_ceil_right]->tickOrient() = 
		axes_[KcAxis::k_ceil_right]->labelOrient() = KcAxis::vec3::unitX();
	axes_[KcAxis::k_ceil_left]->tickOrient() = 
		axes_[KcAxis::k_ceil_left]->labelOrient() = -KcAxis::vec3::unitX();

	for(unsigned i = 0; i < std::size(axes_); i++)
	    axes_[i]->visible() = false;

	axes_[KcAxis::k_near_bottom]->visible() = true;
	axes_[KcAxis::k_near_left]->visible() = true;
	axes_[KcAxis::k_floor_right]->visible() = true;


	// 初始化6个grid平面
	planes_[KcCoordPlane::k_back] = std::make_shared<KcCoordPlane>(KcCoordPlane::k_back,
		axes_[KcAxis::k_far_bottom], axes_[KcAxis::k_far_top], 
		axes_[KcAxis::k_far_left], axes_[KcAxis::k_far_right]);

	planes_[KcCoordPlane::k_front] = std::make_shared<KcCoordPlane>(KcCoordPlane::k_front,
		axes_[KcAxis::k_near_bottom], axes_[KcAxis::k_near_top], 
		axes_[KcAxis::k_near_left], axes_[KcAxis::k_near_right]);

	planes_[KcCoordPlane::k_left] = std::make_shared<KcCoordPlane>(KcCoordPlane::k_left,
		axes_[KcAxis::k_far_left], axes_[KcAxis::k_near_left],
		axes_[KcAxis::k_floor_left], axes_[KcAxis::k_ceil_left]);

	planes_[KcCoordPlane::k_right] = std::make_shared<KcCoordPlane>(KcCoordPlane::k_right,
		axes_[KcAxis::k_far_right], axes_[KcAxis::k_near_right],
		axes_[KcAxis::k_floor_right], axes_[KcAxis::k_ceil_right]);

	planes_[KcCoordPlane::k_ceil] = std::make_shared<KcCoordPlane>(KcCoordPlane::k_ceil,
		axes_[KcAxis::k_far_top], axes_[KcAxis::k_near_top],
		axes_[KcAxis::k_ceil_left], axes_[KcAxis::k_ceil_right]);

	planes_[KcCoordPlane::k_floor] = std::make_shared<KcCoordPlane>(KcCoordPlane::k_floor,
		axes_[KcAxis::k_far_bottom], axes_[KcAxis::k_near_bottom],
		axes_[KcAxis::k_floor_left], axes_[KcAxis::k_floor_right]);

	for (unsigned i = 0; i < std::size(planes_); i++) {
		planes_[i]->visible() = false;
		planes_[i]->minorVisible() = false;
	}

	planes_[KcCoordPlane::k_back]->visible() = true;
	planes_[KcCoordPlane::k_left]->visible() = true;
	planes_[KcCoordPlane::k_floor]->visible() = true;
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

	axes_[KcAxis::k_far_bottom]->setRange(lower.x(), upper.x());
	axes_[KcAxis::k_far_bottom]->setExtend(lower, p5);
	axes_[KcAxis::k_far_top]->setRange(lower.x(), upper.x());
	axes_[KcAxis::k_far_top]->setExtend(p1, p6);
	axes_[KcAxis::k_near_top]->setRange(lower.x(), upper.x());
	axes_[KcAxis::k_near_top]->setExtend(p2, upper);
	axes_[KcAxis::k_near_bottom]->setRange(lower.x(), upper.x());
	axes_[KcAxis::k_near_bottom]->setExtend(p3, p4);

	axes_[KcAxis::k_far_left]->setRange(lower.y(), upper.y());
	axes_[KcAxis::k_far_left]->setExtend(lower, p1);
	axes_[KcAxis::k_far_right]->setRange(lower.y(), upper.y());
	axes_[KcAxis::k_far_right]->setExtend(p5, p6);
	axes_[KcAxis::k_near_right]->setRange(lower.y(), upper.y());
	axes_[KcAxis::k_near_right]->setExtend(p4, upper);
	axes_[KcAxis::k_near_left]->setRange(lower.y(), upper.y());
	axes_[KcAxis::k_near_left]->setExtend(p3, p2);

	axes_[KcAxis::k_floor_left]->setRange(lower.z(), upper.z());
	axes_[KcAxis::k_floor_left]->setExtend(lower, p3);
	axes_[KcAxis::k_floor_right]->setRange(lower.z(), upper.z());
	axes_[KcAxis::k_floor_right]->setExtend(p5, p4);
	axes_[KcAxis::k_ceil_right]->setRange(lower.z(), upper.z());
	axes_[KcAxis::k_ceil_right]->setExtend(p6, upper);
	axes_[KcAxis::k_ceil_left]->setRange(lower.z(), upper.z());
	axes_[KcAxis::k_ceil_left]->setExtend(p1, p2);
}


KcCoord3d::point3 KcCoord3d::lower() const
{
	return { axes_[KcAxis::k_far_bottom]->lower(), 
		axes_[KcAxis::k_far_left]->lower(), 
		axes_[KcAxis::k_floor_left]->lower() 
	};
}


KcCoord3d::point3 KcCoord3d::upper() const
{
	return { axes_[KcAxis::k_far_bottom]->upper(), 
		axes_[KcAxis::k_far_left]->upper(), 
		axes_[KcAxis::k_floor_left]->upper() 
	};
}


KcCoord3d::aabb_t KcCoord3d::boundingBox() const
{
	return { lower(), upper() };
}


void KcCoord3d::forAxis(std::function<bool(KcAxis& axis)> fn) const
{
	for (unsigned i = 0; i < std::size(axes_); i++)
		if (!fn(*axes_[i]))
			break;
}


void KcCoord3d::forPlane(std::function<bool(KcCoordPlane& plane)> fn) const
{
	for (unsigned i = 0; i < std::size(planes_); i++)
		if (!fn(*planes_[i]))
			break;
}


KcCoord3d::rect_t KcCoord3d::getPlotRect() const
{
	return innerRect();
}


void KcCoord3d::placeElement(KvLayoutElement* ele, KeAlignment loc)
{
	assert(!isAncestorOf(ele));

/*	if (loc.inner()) {
		plane_->append(ele);
	}
	else {
		if (loc & KeAlignment::k_horz_first) {
			if (loc & KeAlignment::k_left)
				KuLayoutHelper::placeLeft(plane_.get(), ele, -1);
			else if (loc & KeAlignment::k_right)
				KuLayoutHelper::placeRight(plane_.get(), ele, -1);
			else
				assert(false);
		}
		else {
			if (loc & KeAlignment::k_top)
				KuLayoutHelper::placeTop(plane_.get(), ele, -1);
			else if (loc & KeAlignment::k_bottom)
				KuLayoutHelper::placeBottom(plane_.get(), ele, -1);
			else
				assert(false);
		}
	}*/
}
