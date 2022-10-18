#pragma once
#include "KvCoord.h"
#include <list>
#include <memory>

class KcAxis;
class KcCoordPlane;

// ��ά����ϵʵ��

class KcCoord2d : public KvCoord
{
public:
	using float_t = typename KvCoord::float_t;
	using point2 = KtPoint<float_t, 2>;
	using axis_ptr = std::shared_ptr<KcAxis>;
	using axis_list = std::list<axis_ptr>;
	using grid_plane_ptr = std::shared_ptr<KcCoordPlane>;

	KcCoord2d();
	KcCoord2d(const point2& lower, const point2& upper);

	void setExtents(const point3& lower, const point3& upper) override;

	point3 lower() const override;
	point3 upper() const override;

	void forAxis(std::function<bool(KcAxis& axis)>) const override;

	void forPlane(std::function<bool(KcCoordPlane& plane)>) const override;

	axis_list& axes(int type) { return axes_[type]; }

	// ʵ�ֻ���Ľӿ�

	void draw(KvPaint*) const override;

private:
	axis_list axes_[4];
	grid_plane_ptr plane_;
};
