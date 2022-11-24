#pragma once
#include <functional>
#include "KvRenderable.h"
#include "KtMargins.h"

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

	// 以坐标系AABB的中心点为基准，对各坐标轴的extent/range进行等比例缩放
	// 即坐标系缩放前后，AABB的中心点保持不变
	// factor=1时，坐标系不缩放
	// factor=0时，坐标系收缩到中心点
	void zoom(float_t factor);

	// 轮询坐标轴
	virtual void forAxis(std::function<bool(KcAxis& axis)>) const = 0;

	// 轮询坐标平面
	virtual void forPlane(std::function<bool(KcCoordPlane& plane)>) const = 0;

	virtual KtMargins<float_t> calcMargins(KvPaint*) const = 0;

	// 提供基类接口的缺省实现

	void draw(KvPaint*) const override;

	aabb_type boundingBox() const override;
};
