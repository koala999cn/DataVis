#include "KcCoord3d.h"
#include "KcAxis.h"
#include "KcCoordPlane.h"
#include "KvPaint.h"
#include <assert.h>


KcCoord3d::KcCoord3d()
	: KcCoord3d(point3(0), point3(1))
{
	putAt(0, 0, new KcLayoutOverlay); // �˴�ѹ��һ��dummyԪ�أ�������Ԫ�ص�grid���ֳߴ�ʼ��Ϊ0
	layCoord_ = std::make_unique<KcLayoutOverlay>();
}


KcCoord3d::KcCoord3d(const point3& lower, const point3& upper)
	: KvCoord("CoordSystem3d")
{
	int dim[12];
	dim[KcAxis::k_near_top] = dim[KcAxis::k_near_bottom] 
		= dim[KcAxis::k_far_top] = dim[KcAxis::k_far_bottom] = 0;
	dim[KcAxis::k_near_left] = dim[KcAxis::k_near_right] 
		= dim[KcAxis::k_far_left] = dim[KcAxis::k_far_right] = 1;
	dim[KcAxis::k_floor_left] = dim[KcAxis::k_floor_right]
		= dim[KcAxis::k_ceil_left] = dim[KcAxis::k_ceil_right] = 2;

	// ��ʼ��12����������
	for (unsigned i = 0; i < std::size(axes_); i++)
		axes_[i] = std::make_shared<KcAxis>(KcAxis::KeType(i), dim[i], true);

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

	static const char* title[] = { "X", "Y", "Z" };
	for (unsigned i = 0; i < std::size(axes_); i++) {
		axes_[i]->visible() = false;
		axes_[i]->title() = title[axes_[i]->dim()];
	}

	axes_[KcAxis::k_near_bottom]->visible() = true;
	axes_[KcAxis::k_near_left]->visible() = true;
	axes_[KcAxis::k_floor_right]->visible() = true;


	// ��ʼ��6��gridƽ��
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

	if (!layCoord_->empty()) { // ��Ҫʱ�ż���rcCoord_

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

	ele->align() = loc; // TODO�� ������ȫһ�£���ʱ�򵥴���
	layCoord_->append(ele);
}

