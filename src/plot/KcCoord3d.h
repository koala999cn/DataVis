#pragma once
#include "KvCoord.h"
#include <vector>
#include <memory>
#include "layout/KcLayoutOverlay.h"


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

	void forAxis(std::function<bool(KcAxis& axis)>) const override;

	void forPlane(std::function<bool(KcCoordPlane& plane)>) const override;

	std::shared_ptr<KcAxis> defaultAxis(unsigned dim) const final;

	KvLayoutElement* getFrame() const final;

	void arrange_(int dim, float_t lower, float_t upper) override final;

	axis_ptr& axis(int type) { return axes_[type]; }
	axis_ptr axis(int type) const { return axes_[type]; }


private:
	size_t calcSize_(void* cxt) const final;

private:
	axis_ptr axes_[12];
	grid_plane_ptr planes_[6];
	std::unique_ptr<KcLayoutOverlay> layCoord_;
	mutable rect_t rcCoord_; // coord3d的屏幕坐标rect，用于作为legend和colorbar的基准. 由calcSize_负责计算
};
