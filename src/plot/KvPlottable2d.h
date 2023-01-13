#pragma once
#include "KvPlottable.h"
#include "KvPaint.h"

class KvSampled;

// image图的基类，主要处理grid（sampled2d）数据，用于绘制热图heatmap、曲面图surface等
// 兼容2d和3d模式

class KvPlottable2d : public KvPlottable
{
	using super_ = KvPlottable;

public:

	using super_::super_;

	const color4f& minorColor() const override;

	void setMinorColor(const color4f& minor) override;

	bool showBorder() const { return showBorder_; }
	bool& showBorder() { return showBorder_; }

	const KpPen& borderPen() const { return borderPen_; }
	KpPen& borderPen() { return borderPen_; }

protected:

	void drawDiscreted_(KvPaint*, KvDiscreted*) const override;

	using point_getter2 = typename KvPaint::point_getter2;
	virtual void drawImpl_(KvPaint*, point_getter2, unsigned nx, unsigned ny, unsigned channels) const = 0;

private:

	// useDefaultZ为true时调用， 绘制二维的colormap图（使用缺省z值）
	// 使用最高维度的数据值进行颜色插值
	void draw1d_(KvPaint*, KvSampled*) const;

	// useDefaultZ为false时调用， 绘制三维的colormap图
	// 使用最高维度的数据值进行颜色插值
	void draw2d_(KvPaint*, KvSampled*) const;

private:

	// 该标记为真时，将用默认Z值替换原来的z值，可用来在3d空间绘制二维的colormap图
	bool useDefaultZ_{ false }; 

	bool showBorder_{ false };
	KpPen borderPen_;
};
