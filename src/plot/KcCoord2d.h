#pragma once
#include "KvCoord.h"
#include <list>
#include <memory>
#include "KtMargins.h"

class KcAxis;
class KcCoordPlane;
class KvPlottable; // for split axis

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

	void forAxis(std::function<bool(KcAxis& axis)>) const final;

	void forPlane(std::function<bool(KcCoordPlane& plane)>) const final;

	std::shared_ptr<KcAxis> defaultAxis(unsigned dim) const final;

	rect_t getPlotRect() const final;

	void placeElement(KvLayoutElement* ele, KeAlignment loc) final;

	axis_list& axes(int type) { return axes_[type]; }

	// @mode: =0表示取消分离，使用主坐标轴; =1表示分离坐标轴到bottom或left，=2表示分离坐标轴到top或right
	void splitAxis(KvPlottable* plt, unsigned dim, int mode);

private:

	KtMargins<float_t> calcMargins_(KvPaint*) const;

	size_t calcSize_(void* cxt) const final;

	void fixMargins_();

	void addSplitAxis_(const axis_ptr& axis);
	void eraseSplitAxis_(const axis_ptr& axis);

private:
	axis_list axes_[4];
	grid_plane_ptr plane_;
};
