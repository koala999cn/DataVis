#pragma once
#include <memory>
#include <vector>
#include "KtColor.h"
#include "KvPlottable.h"
#include "KcCoordSystem.h"
#include "KtMatrix4.h"

class KvPaint; // 用来执行具体的3d绘制指令

class KvPlot3d
{
public:

	using mat4 = mat4f<false>;
	using point3 = point3f;

	KvPlot3d(std::shared_ptr<KvPaint> paint);

	/// 抽象接口

	virtual void setVisible(bool b) = 0;

	virtual bool visible() const = 0;

	virtual color4f background() const = 0;
	virtual void setBackground(const color4f& clr) = 0;

	virtual mat4 viewMatrix() const = 0;
	virtual void setViewMatrix(const mat4&) = 0;

	virtual mat4 projMatrix() const = 0;
	virtual void setProjMatrix(const mat4&) = 0;

	virtual void update(); // 更新绘图


	/// 内置实现的坐标系和绘图元素管理

	KcCoordSystem& coordSystem() {
		return *coord_.get();
	}

	unsigned plottableCount() const {
		return plottables_.size();
	}

	KvPlottable* plottable(unsigned idx);

	// 接管plot的控制权
	void addPlottable(KvPlottable* plot);

	bool isOrtho() const { return ortho_; }
	void setOrtho(bool b) { ortho_ = b; }

	bool isAutoFit() const { return autoFit_; }
	void setAutoFit(bool b) { autoFit_ = b; }

	bool isIsometric() const { return isometric_; }
	void setIsometric(bool b) { isometric_ = b; }

	double getZoom() const { return zoom_; }
	void setZoom(double zoom) { zoom_ = zoom; }
	void zoom(double factor) { zoom_ *= factor; }

	point3 getScale() const { return scale_; }
	void setScale(const point3& scale) { scale_ = scale; }
	void scale(const point3& factor) { scale_ *= factor; }

	point3 getShift() const { return shift_; }
	void setShift(const point3& shift) { shift_ = shift; }
	void shift(const point3& delta) { shift_ += delta; }

	point3 getRotate() const { return rotate_; }
	void setRotate(const point3& rotate) { rotate_ = rotate; }
	void rotate(const point3& delta) { rotate_ += delta; }

protected:
	virtual void autoProject_() = 0;
	virtual void fitData_();
	
protected:
	std::shared_ptr<KvPaint> paint_; // 由用户创建并传入
	std::unique_ptr<KcCoordSystem> coord_; // 内置创建并管理
	std::vector<std::unique_ptr<KvPlottable>> plottables_; // 由用户通过类成员方法管理
	
	bool ortho_{ true }; // 正交投影 vs. 透视投影
	bool autoFit_{ true }; // 若true，则每次update都将根据数据range自动调整坐标系extents

	// 以下参数用于调整摄像机modelview矩阵
	double zoom_{ 1 };
	point3 scale_{ 1, 1, 1 };
	point3 shift_{ 0, 0, 0 };
	point3 rotate_{ 0, 0, 0 };

	bool isometric_{ false }; // 若true，则保持坐标系的等比性，即各轴的单元长度保持一致
	                          // 若false，则优先考虑布局美观性，坐标系的初始透视结果始终为正方形（后续可通过scale_参数进行拉伸）
};
