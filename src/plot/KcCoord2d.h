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
	using float_type = typename KvRenderable::float_type;
	using point2 = KtPoint<float_type, 2>;
	using axis_ptr = std::shared_ptr<KcAxis>;
	using aixs_list = std::list<axis_ptr>;

	enum KeAxis 
	{
		k_left,
		k_right,
		k_top,
		k_bottom,
		k_custom
	};

	KcCoord2d();
	KcCoord2d(const point2& lower, const point2& upper);

	void setExtents(const point2& lower, const point2& upper);

	point2 lower() const; // the lower conner
	point2 upper() const; // the upper conner

	aixs_list& axes(KeAxis kind) { return axes_[kind]; }

	// 实现基类的接口

	void draw(KvPaint*) const override;

	aabb_type boundingBox() const override;

private:
	aixs_list axes_[5];
};
