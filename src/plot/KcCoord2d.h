#pragma once
#include "KvRenderable.h"
#include <vector>
#include <memory>
#include "KtPoint.h"

class KcAxis;

// 二维坐标系实现

class KcCoord2d : public KvRenderable
{
public:
	using float_type = typename KvRenderable::float_type;
	using point2 = KtPoint<float_type, 2>;

	enum KeAxis 
	{
		k_left,
		k_right,
		k_top,
		k_bottom
	};

	KcCoord2d();
	KcCoord2d(const point2& lower, const point2& upper);

	void setExtents(const point2& lower, const point2& upper);

	point2 lower() const; // the lower conner
	point2 upper() const; // the upper conner

	axis_ptr& axis(KeAxis id) { return axes_[id]; }
	axis_ptr axis(KeAxis id) const { return axes_[id]; }

	// 以坐标系AABB的中心点为基准，对各坐标轴的extent/range进行等比例缩放
	// 即坐标系缩放前后，AABB的中心点保持不变
	// factor=1时，坐标系不缩放
	// factor=0时，坐标系收缩到中心点
	void zoom(double factor);

	void draw(KvPaint*) const override;

private:
	axis_ptr axes_[4];
};
