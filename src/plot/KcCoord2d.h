#pragma once
#include "KvCoord.h"
#include <list>
#include <memory>
#include "layout/KgLayoutManager.h"

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

	KcCoord2d();
	KcCoord2d(const point2& lower, const point2& upper);

	virtual ~KcCoord2d();

	void setExtents(const point3& lower, const point3& upper) override;

	point3 lower() const override;
	point3 upper() const override;

	void forAxis(std::function<bool(KcAxis& axis)>) const override;

	void forPlane(std::function<bool(KcCoordPlane& plane)>) const override;

	KtMargins<float_t> calcMargins(KvPaint*) const override;

	rect_t getPlotRect() const override;

	axis_list& axes(int type) { return axes_[type]; }

	// 实现基类的接口

	void draw(KvPaint*) const final;

	void arrange(const rect_t& rc) final;

	point2i extraShares() const final;

private:

	size_t calcSize_(void* cxt) const final;

private:
	axis_list axes_[4];
	grid_plane_ptr plane_;
	mutable KgLayoutManager layMgr_;
};
