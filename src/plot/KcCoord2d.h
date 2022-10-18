#pragma once
#include "KvCoord.h"
#include <list>
#include <memory>

class KcAxis;
class KcCoordPlane;

// 二维坐标系实现

class KcCoord2d : public KvCoord
{
public:
	using float_t = typename KvCoord::float_t;
	using point2 = KtPoint<float_t, 2>;
	using axis_ptr = std::shared_ptr<KcAxis>;
	using axis_list = std::list<axis_ptr>;
	using grid_plane_ptr = std::shared_ptr<KcCoordPlane>;

	enum KeAxisType
	{
		k_axis_left,
		k_axis_right,
		k_axis_top,
		k_axis_bottom,
		k_axis_custom,
		k_axis_type_count
	};

	KcCoord2d();
	KcCoord2d(const point2& lower, const point2& upper);

	void setExtents(const point3& lower, const point3& upper) override;

	point3 lower() const override;
	point3 upper() const override;

	void forAxis(std::function<bool(KcAxis& axis)>) const override;

	void forPlane(std::function<bool(KcCoordPlane& plane)>) const override;

	axis_list& axes(KeAxisType type) { return axes_[type]; }

	// 实现基类的接口

	void draw(KvPaint*) const override;

private:
	axis_list axes_[k_axis_type_count];
	grid_plane_ptr plane_;
};
