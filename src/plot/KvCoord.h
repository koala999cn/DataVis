#pragma once
#include <functional>
#include "KvRenderable.h"
#include "KtMatrix4.h"
#include "layout/KcLayoutGrid.h"

class KcAxis; 
class KcCoordPlane;

// 绘图坐标系的抽象接口
// 内置坐标系缩放、绘制和坐标轴反转、交换等实现

class KvCoord : public KvRenderable, public KcLayoutGrid
{
public:
	using float_t = typename KvRenderable::float_t;
	using point3 = KtPoint<float_t, 3>;
	using mat4 = KtMatrix4<float_t>;

	using KvRenderable::KvRenderable;

	void setExtents(const point3& lower, const point3& upper);

	// the lower conner
	point3 lower() const { return extent_[0]; }

	// the upper conner
	point3 upper() const { return extent_[1]; }

	// 轮询坐标轴
	virtual void forAxis(std::function<bool(KcAxis& axis)>) const = 0;

	// 轮询坐标平面
	virtual void forPlane(std::function<bool(KcCoordPlane& plane)>) const = 0;

	// 返回实际的绘图区域，paint将此设置为viewport，之后绘制plottables
	virtual rect_t getPlotRect() const = 0;

	virtual void placeElement(KvLayoutElement* ele, KeAlignment loc) = 0;

	// 返回dim维度的缺省主坐标轴，用于设置plottable的axis
	virtual std::shared_ptr<KcAxis> defaultAxis(unsigned dim) const = 0;

	// the center point
	point3 center() const {
		return (upper() + lower()) / 2;
	}

	// 以坐标系AABB的中心点为基准，对各坐标轴的extent/range进行等比例缩放
	// 即坐标系缩放前后，AABB的中心点保持不变
	// factor=1时，坐标系不缩放
	// factor=0时，坐标系收缩到中心点
	void zoom(float_t factor);

	/// 以下3个成员方法只针对主坐标轴进行操作

	// 反转dim维度的主坐标轴
	void inverseAxis(int dim, bool inv); 

	// dim维度坐标轴是否反转
	bool axisInversed(int dim) const { return inv_[dim]; }

	// 有任意坐标轴反转，则返回true
	bool axisInversed() const {
		return axisInversed(0) || axisInversed(1) || axisInversed(2);
	}

	enum KeAxisSwapStatus
	{
		k_axis_swap_none,
		k_axis_swap_xy,
		k_axis_swap_xz,
		k_axis_swap_yz
	};

	void swapAxis(KeAxisSwapStatus status);
	KeAxisSwapStatus axisSwapped() const { return swapStatus_; }

	mat4 axisInverseMatrix() const;

	const mat4& axisSwapMatrix() const;

	// 返回坐标轴反转和交换所构成的变换矩阵，可用于绘制plottable
	// mat4 localMatrix() const;

	// 提供基类接口的缺省实现

	void draw(KvPaint*) const override;

	// 返回局部坐标的aabb
	aabb_t boundingBox() const override;

private:

	mat4 axisReflectMatrix_(int dim) const;

	void resetAxisExtent_(KcAxis& axis, bool swap) const;

private:
	point3 extent_[2]; // 主坐标轴的范围：extent_[0] = lower, extent_[1] = upper
	bool inv_[3]{ false }; // 保存主坐标轴（x/y/z）的反转状态，用于快速访问
	KeAxisSwapStatus swapStatus_{ k_axis_swap_none }; // 保存坐标轴交换的状态
};
