#pragma once
#include <functional>
#include "KvRenderable.h"

class KcAxis; 
class KcCoordPlane;

// 绘图坐标系的抽象接口

class KvCoord : public KvRenderable
{
public:
	using point3 = KtPoint<float_t, 3>;

	using KvRenderable::KvRenderable;

	virtual ~KvCoord() {}

	virtual void setExtents(const point3& lower, const point3& upper) = 0;;

	// the lower conner
	virtual point3 lower() const = 0; 

	// the upper conner
	virtual point3 upper() const = 0; 

	// the center point
	point3 center() const {
		return (upper() + lower()) / 2;
	}

	void zoom(float_t factor);

	// 轮询坐标轴
	virtual void forAxis(std::function<bool(KcAxis& axis)>) const = 0;

	// 轮询坐标平面
	virtual void forPlane(std::function<bool(KcCoordPlane& plane)>) const = 0;

	// 提供基类接口的缺省实现

	void draw(KvPaint*) const override;

	aabb_type boundingBox() const override;
};