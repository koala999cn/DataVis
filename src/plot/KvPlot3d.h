#pragma once
#include "KvPlot.h"
#include "KcCoordSystem.h"
#include "KtMatrix4.h"
#include "KvRenderable.h"


class KvPlot3d : public KvPlot
{
public:
	using float_type = typename KvRenderable::float_type;
	using mat4 = KtMatrix4<float_type>;
	using point3 = typename KvRenderable::point3;
	using quat = KtQuaternion<float_type>;

	KvPlot3d(std::shared_ptr<KvPaint> paint);

	/// 抽象接口

	virtual mat4 viewMatrix() const = 0;
	virtual void setViewMatrix(const mat4&) = 0;

	virtual mat4 projMatrix() const = 0;
	virtual void setProjMatrix(const mat4&) = 0;

	void update() override;

	void fitData() override;

	bool ortho() const { return ortho_; }
	bool& ortho() { return ortho_; }

	bool isometric() const { return isometric_; }
	bool& isometric() { return isometric_; }

	float_type zoom() const { return zoom_; }
	float_type& zoom() { return zoom_; }

	const point3& scale() const { return scale_; }
	point3& scale() { return scale_; }

	const point3& shift() const { return shift_; }
	point3& shift() { return shift_; }

	const quat& orient() const { return orient_; }
	quat& orient() { return orient_; }

	KcCoordSystem& coordSystem() { return *coord_.get(); }

protected:
	virtual void autoProject_() = 0;
	
protected:
	std::unique_ptr<KcCoordSystem> coord_; // 内置创建并管理
	
	bool ortho_{ true }; // 正交投影 vs. 透视投影
	
	// 以下参数用于调整摄像机modelview矩阵
	float_type zoom_{ 1 };
	point3 scale_{ 1, 1, 1 }; 
	point3 shift_{ 0, 0, 0 };
	quat orient_{ 0, 0, 0, 1 }; // 摄像机的方位

	bool isometric_{ false }; // 若true，则保持坐标系的等比性，即各轴的单元长度保持一致
	                          // 若false，则优先考虑布局美观性，坐标系的初始透视结果始终为正方形（后续可通过scale_参数进行拉伸）
};
