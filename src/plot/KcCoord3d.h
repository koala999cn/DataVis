#pragma once
#include "KvCoord.h"
#include <vector>
#include <memory>


// 三维坐标系实现，由12根坐标轴和6个平面构成
// 默认X轴向右，Y轴向上，Z轴向外

class KcCoord3d : public KvCoord
{
public:
	using axis_ptr = std::shared_ptr<KcAxis>;
	using grid_plane_ptr = std::shared_ptr<KcCoordPlane>;
	

public:

	KcCoord3d();
	KcCoord3d(const point3& lower, const point3& upper);

	void setExtents(const point3& lower, const point3& upper) override;

	point3 lower() const override;
	point3 upper() const override;

	aabb_t boundingBox() const override;

	void forAxis(std::function<bool(KcAxis& axis)>) const override;

	void forPlane(std::function<bool(KcCoordPlane& plane)>) const override;

	KtMargins<float_t> calcMargins(KvPaint*) const override;

	rect_t getPlotRect() const override;

	void placeElement(KvLayoutElement* ele, KeAlignment loc) final;

	axis_ptr& axis(int type) { return axes_[type]; }
	axis_ptr axis(int type) const { return axes_[type]; }


private:
	axis_ptr axes_[12];
	grid_plane_ptr planes_[6];
};
