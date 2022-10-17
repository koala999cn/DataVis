#pragma once
#include "KvRenderable.h"
#include <vector>
#include <memory>
#include "KtPoint.h"

class KcAxis;
class KcGridPlane;

// 三维坐标系实现，由12根坐标轴和6个平面构成
// 默认X轴向右，Y轴向上，Z轴向外

class KcCoord3d : public KvRenderable
{
public:
	using point3 = KtPoint<float_t, 3>;
	using axis_ptr = std::shared_ptr<KcAxis>;
	using grid_plane_ptr = std::shared_ptr<KcGridPlane>;
	

	// 12根坐标轴的id，用于索引axes_
	enum KeAxis
	{
		k_x0, /*bottom-far*/	k_y0, /*left-far*/		k_z0, /*left-bottom*/

		k_x1, /*top-far*/		k_y1, /*right-far*/		k_z1, /*right-bottom*/

		k_x2, /*top-near*/		k_y2, /*right-near*/	k_z2, /*right-top*/

		k_x3, /*bottom_near*/	k_y3, /*left_near*/		k_z3  /*left_top*/
	};

	// 6个可以绘制grid的平面
	enum KeGrid
	{
		k_xy_back,
		k_xy_front, 

		k_yz_left, 
		k_yz_right,

		k_xz_ceil,
		k_xz_floor,
	};

public:

	KcCoord3d();
	KcCoord3d(const point3& lower, const point3& upper);

	void setExtents(const point3& lower, const point3& upper);

	point3 lower() const; // the lower conner
	point3 upper() const; // the upper conner

	point3 center() const; // the center point

	axis_ptr& axis(KeAxis id) { return axes_[id]; }
	axis_ptr axis(KeAxis id) const { return axes_[id]; }

	// 以坐标系AABB的中心点为基准，对各坐标轴的extent/range进行等比例缩放
	// 即坐标系缩放前后，AABB的中心点保持不变
	// factor=1时，坐标系不缩放
	// factor=0时，坐标系收缩到中心点
	void zoom(double factor);

	void draw(KvPaint*) const override;

	aabb_type boundingBox() const override;

private:
	axis_ptr axes_[12];
	grid_plane_ptr planes_[6];
};
