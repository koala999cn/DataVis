#pragma once
#include "KvPlot.h"
#include "KcCoordSystem.h"
#include "KtMatrix4.h"


class KvPlot3d : public KvPlot
{
public:

	using mat4 = mat4f<>;
	using point3 = point3f;

	KvPlot3d(std::shared_ptr<KvPaint> paint);

	/// 抽象接口

	virtual mat4 viewMatrix() const = 0;
	virtual void setViewMatrix(const mat4&) = 0;

	virtual mat4 projMatrix() const = 0;
	virtual void setProjMatrix(const mat4&) = 0;

	void update() override;

	void fitData() override;

	bool isOrtho() const { return ortho_; }
	void setOrtho(bool b) { ortho_ = b; }

	bool isIsometric() const { return isometric_; }
	void setIsometric(bool b) { isometric_ = b; }

	KcCoordSystem& coordSystem() {
		return *coord_.get();
	}

protected:
	virtual void autoProject_() = 0;
	
protected:
	std::unique_ptr<KcCoordSystem> coord_; // 内置创建并管理
	
	bool ortho_{ true }; // 正交投影 vs. 透视投影
	
	// 以下参数用于调整摄像机modelview矩阵
	float zoom_{ 1 };
	point3 scale_{ 1, 1, 1 }; 
	point3 shift_{ 0, 0, 0 };
	quatf orient_{ 0, 0, 0, 1 }; // 摄像机的方位

	bool isometric_{ false }; // 若true，则保持坐标系的等比性，即各轴的单元长度保持一致
	                          // 若false，则优先考虑布局美观性，坐标系的初始透视结果始终为正方形（后续可通过scale_参数进行拉伸）
};
