#include "KcCoord3d.h"
#include "KcAxis.h"
#include "KcCoordPlane.h"
#include "KvPaint.h"
#include <assert.h>
#include "layout/KcLayoutOverlay.h"


KcCoord3d::KcCoord3d()
	: KcCoord3d(point3(0), point3(1))
{
	putAt(0, 0, new KcLayoutOverlay); // 此处压入一个dummy元素，否则零元素的grid布局尺寸始终为0
	layCoord_ = std::make_unique<KcLayoutOverlay>();
}


KcCoord3d::KcCoord3d(const point3& lower, const point3& upper)
	: KvCoord("CoordSystem3d")
{
	int dim[12];
	dim[KcAxis::k_near_top] = dim[KcAxis::k_near_bottom] 
		= dim[KcAxis::k_far_top] = dim[KcAxis::k_far_bottom] = 0;
	dim[KcAxis::k_near_left] = dim[KcAxis::k_near_right] 
		= dim[KcAxis::k_near_left] = dim[KcAxis::k_near_right] = 1;
	dim[KcAxis::k_floor_left] = dim[KcAxis::k_floor_right]
		= dim[KcAxis::k_ceil_left] = dim[KcAxis::k_ceil_right] = 2;

	// 初始化12根主坐标轴
	for (unsigned i = 0; i < std::size(axes_); i++)
		axes_[i] = std::make_shared<KcAxis>(KcAxis::KeAxisType(i), dim[i], true);

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


KcCoord3d::size_t KcCoord3d::calcSize_(void* cxt) const
{
	rcCoord_.setNull();

	layCoord_->calcSize(cxt);

	if (!layCoord_->empty()) { // 需要时才计算rcCoord_

		aabb_t box(lower(), upper());
		auto corns = box.allCorners();

		for (auto& i : corns) {
			i = ((KvPaint*)cxt)->projectp(i);
			rcCoord_.merge({ i.x(), i.y() });
		}
	}

	return __super::calcSize_(cxt);
}


void KcCoord3d::arrange(const rect_t& rc)
{
	__super::arrange(rc);

	if (!layCoord_->empty()) {
		auto rcCoord = rcCoord_;
		for (unsigned i = 0; i < 2; i++)
			if (rc.extent(i) == 0) rcCoord.setExtent(i, 0);
		layCoord_->arrange(rcCoord);
	}
}


void KcCoord3d::placeElement(KvLayoutElement* ele, KeAlignment loc)
{
	assert(!isAncestorOf(ele));

	ele->align() = loc; // TODO： 并不完全一致，暂时简单处理
	layCoord_->append(ele);
}

