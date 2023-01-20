#include "KcCoord3d.h"
#include "KcAxis.h"
#include "KcCoordPlane.h"
#include "KvPaint.h"
#include <assert.h>


KcCoord3d::KcCoord3d()
	: KcCoord3d(point3(0), point3(1))
{

}


KcCoord3d::KcCoord3d(const point3& lower, const point3& upper)
	: KvCoord("CoordSystem3d")
{
	putAt(0, 0, new KcLayoutOverlay); // 此处压入一个dummy元素，否则零元素的grid布局尺寸始终为0
	layCoord_ = std::make_unique<KcLayoutOverlay>();

	int dim[12];
	dim[KcAxis::k_near_top] = dim[KcAxis::k_near_bottom] 
		= dim[KcAxis::k_far_top] = dim[KcAxis::k_far_bottom] = 0;
	dim[KcAxis::k_near_left] = dim[KcAxis::k_near_right] 
		= dim[KcAxis::k_far_left] = dim[KcAxis::k_far_right] = 1;
	dim[KcAxis::k_floor_left] = dim[KcAxis::k_floor_right]
		= dim[KcAxis::k_ceil_left] = dim[KcAxis::k_ceil_right] = 2;

	// 初始化12根主坐标轴
	for (unsigned i = 0; i < std::size(axes_); i++)
		axes_[i] = std::make_shared<KcAxis>(KcAxis::KeType(i), dim[i], true);

	setExtents(lower, upper);

	static const char* title[] = { "X", "Y", "Z" };
	for (unsigned i = 0; i < std::size(axes_); i++) {
		axes_[i]->visible() = false;
		axes_[i]->title() = title[axes_[i]->dim()];
	}

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
		axes_[KcAxis::k_floor_left], axes_[KcAxis::k_ceil_left],
		axes_[KcAxis::k_far_left], axes_[KcAxis::k_near_left]);

	planes_[KcCoordPlane::k_right] = std::make_shared<KcCoordPlane>(KcCoordPlane::k_right,
		axes_[KcAxis::k_floor_right], axes_[KcAxis::k_ceil_right],
		axes_[KcAxis::k_far_right], axes_[KcAxis::k_near_right]);

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

	forAxis([cxt](KcAxis& axis) {
		if (axis.visible())
		    axis.calcSize(cxt);
		return true;
		});

	if (!layCoord_->empty()) { // 需要时才计算rcCoord_

		auto aabb = KtAABB<float_t, 3>(lower(), upper());
		//forAxis([&aabb](KcAxis& axis) {
		//	if (axis.visible())
		//		aabb.merge(axis.boundingBox());
		//	return true;
		//	});
		// TODO: boundingBox返回局部坐标还是世界坐标？？？

		auto corns = aabb.allCorners();
		rcCoord_.setNull();

		for (auto& i : corns) {
			i = ((KvPaint*)cxt)->projectp(i);
			rcCoord_.merge({ i.x(), i.y() });
		}
	}

	return __super::calcSize_(cxt);
}


void KcCoord3d::arrange_(int dim, float_t lower, float_t upper)
{
	__super::arrange_(dim, lower, upper);

	if (!layCoord_->empty()) {
		//auto rcCoord = rcCoord_;
		//for (unsigned i = 0; i < 2; i++)
		//	if (rc.extent(i) == 0) rcCoord.setExtent(i, 0);
		layCoord_->arrange_(dim, lower, upper);
	}
}


void KcCoord3d::placeElement(KvLayoutElement* ele, KeAlignment loc)
{
	assert(!isAncestorOf(ele));

	ele->align() = loc; // TODO： 并不完全一致，暂时简单处理

	for(unsigned i = 0; i < layCoord_->size(); i++)
		if (layCoord_->getAt(i) == 0) {
			layCoord_->setAt(i, ele);
			return;
		}

	layCoord_->append(ele);
}

