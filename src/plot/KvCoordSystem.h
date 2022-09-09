#pragma once
#include "KvRenderable.h"
#include <vector>
#include <memory>
#include "KtVector3.h"
#include "KtVector4.h"

class KgAxis;

// 坐标系基类
// 默认X轴向右，Y轴向上，Z轴向外

class KvCoordSystem : public KvRenderable
{
public:
	using axis_ptr = std::shared_ptr<KgAxis>;
	using vec3 = KtVector3<double>;
	using vec4 = KtVector4<double>;

	KvCoordSystem(const vec3& lower, const vec3& upper);

	// 12根坐标轴的id，用于索引axes_
	enum KeAxis
	{
		k_x0, // bottom-far
		k_y0, // left-far
		k_z0, // left-bottom

		k_x1, // top-far,
		k_y1, // right-far,
		k_z1, // right-bottom,

		k_x2, // top-near,
		k_y2, // right-near,
		k_z2, // right-top,

		k_x3, // bottom_near,
		k_y3, // left_near,
		k_z3  // left_top
	};

	axis_ptr& axis(int id) { return axes_[id]; }
	axis_ptr axis(int id) const { return axes_[id]; }

	void setRange(const vec3& lower, const vec3& upper);

	vec3 lowerCorner() const;
	vec3 upperCorner() const;

	void setVisible(bool b) override;

	bool visible() const override;

private:
	vec4 bkgnd_;
	bool visible_;

	std::vector<axis_ptr> axes_;
};
