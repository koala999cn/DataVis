#pragma once
#include "KvRenderable.h"
#include <vector>
#include <memory>
#include "KtPoint.h"

class KcAxis;
class KcGridPlane;

// 坐标系实现类，由12根坐标轴和6个平面构成
// 默认X轴向右，Y轴向上，Z轴向外

class KcCoordSystem : public KvRenderable
{
public:
	using axis_ptr = std::shared_ptr<KcAxis>;
	using grid_plane_ptr = std::shared_ptr<KcGridPlane>;
	using KvRenderable::aabb_type;

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
		/*
		k_back_h, k_y0_y1 = k_back_h,
		k_back_v, k_x0_x1 = k_back_v,

		k_front_h, k_y2_y3 = k_front_h,
		k_front_v, k_x2_x3 = k_front_v,*/

		k_xy_back,
		//k_x0_y0 = k_back, k_x0_y1 = k_back, k_x1_y0 = k_back, k_x1_y1 = k_back,

		k_xy_front, 
		//k_x2_y2 = k_front, k_x2_y3 = k_front, k_x3_y2 = k_front, k_x3_y3 = k_front,

		k_yz_left, 
		//k_y0_z0 = k_left, k_y0_z3 = k_left, k_y3_z0 = k_left, k_y3_z3 = k_left,

		k_yz_right,
		//k_y1_z1 = k_right, k_y1_z2 = k_right, k_y2_z1 = k_right, k_y2_z2 = k_right,

		k_xz_ceil,
		//k_x1_z2 = k_ceil, k_x1_z3 = k_ceil, k_x2_z2 = k_ceil, k_x2_z3 = k_ceil,

		k_xz_floor,
		//k_x0_z0 = k_floor, k_x0_z1 = k_floor, k_x3_z0 = k_floor, k_x3_z1 = k_floor
	};

public:

	KcCoordSystem();
	KcCoordSystem(const point3& lower, const point3& upper);

	void setExtents(const point3& lower, const point3& upper);

	point3 lower() const; // the lower conner
	point3 upper() const; // the upper conner

	point3 center() const; // the center point

	// 返回aabb的对角线长度
	double diag() const;

	axis_ptr& axis(KeAxis id) { return axes_[id]; }
	axis_ptr axis(KeAxis id) const { return axes_[id]; }

	// 以坐标系AABB的中心点为基准，对各坐标轴的extent/range进行等比例缩放
	// 即坐标系缩放前后，AABB的中心点保持不变
	// factor=1时，坐标系不缩放
	// factor=0时，坐标系收缩到中心点
	void zoom(double factor);

	aabb_type boundingBox() const override;

	void draw(KvPaint3d*) const override;

	// grid相关成员方法
/*
	bool gridVisible(KeGrid id) const {
		return grids_[id].visible_;
	}
	void setGridVisible(KeGrid id, bool b) {
		grids_[id].visible_ = b;
	}

	vec4 gridColor(KeGrid id) const {
		return grids_[id].color_;
	}
	void setGridColor(KeGrid id, const vec4& color) {
		grids_[id].color_ = color;
	}

	int gridStyle(KeGrid id) const {
		return grids_[id].style_;
	}
	void setGridStyle(KeGrid id, int style) {
		grids_[id].style_ = style;
	}

	int gridWidth(KeGrid id) const {
		return grids_[id].width_;
	}
	void setGridWidth(KeGrid id, double width) {
		grids_[id].width_ = width;
	}

	bool subgridVisible(KeGrid id) const {
		return subgrids_[id].visible_;
	}
	void setSubgridVisible(KeGrid id, bool b) {
		subgrids_[id].visible_ = b;
	}

	vec4 subgridColor(KeGrid id) const {
		return subgrids_[id].color_;
	}
	void setSubgridColor(KeGrid id, const vec4& color) {
		subgrids_[id].color_ = color;
	}

	int subgridStyle(KeGrid id) const {
		return subgrids_[id].style_;
	}
	void setSubgridStyle(KeGrid id, int style) {
		subgrids_[id].style_ = style;
	}

	int subgridWidth(KeGrid id) const {
		return subgrids_[id].width_;
	}
	void setSubgridWidth(KeGrid id, double width) {
		subgrids_[id].width_ = width;
	}
*/

private:
	axis_ptr axes_[12];
	grid_plane_ptr planes_[6];
};
