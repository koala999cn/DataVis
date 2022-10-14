#pragma once
#include "KvRenderable.h"
#include <list>
#include <memory>
#include "KtPoint.h"

class KcAxis;

// 二维坐标系实现

class KcCoord2d : public KvRenderable
{
public:
	using float_t = typename KvRenderable::float_t;
	using point2 = KtPoint<float_t, 2>;
	using axis_ptr = std::shared_ptr<KcAxis>;
	using axis_list = std::list<axis_ptr>;

	enum KeAxisType
	{
		k_left,
		k_right,
		k_top,
		k_bottom,
		k_custom,
		k_type_count
	};

	KcCoord2d();
	KcCoord2d(const point2& lower, const point2& upper);

	void setExtents(const point2& lower, const point2& upper);

	point2 lower() const; // the lower conner
	point2 upper() const; // the upper conner

	axis_list& axes(KeAxisType type) { return axes_[type]; }

	// 实现基类的接口

	void draw(KvPaint*) const override;

	aabb_type boundingBox() const override;

private:
	axis_list axes_[k_type_count];
};
