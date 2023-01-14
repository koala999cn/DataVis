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

	aabb_t boundingBox() const override;

	const color4f& minorColor() const override;

	void setMinorColor(const color4f& minor) override;

	bool showBorder() const { return showBorder_; }
	bool& showBorder() { return showBorder_; }

	const KpPen& borderPen() const { return borderPen_; }
	KpPen& borderPen() { return borderPen_; }

	bool forceDefaultZ() const { return forceDefaultZ_; }
	bool& forceDefaultZ() { return forceDefaultZ_; }

protected:

	void drawDiscreted_(KvPaint*, KvDiscreted*) const override;

	using point_getter2 = typename KvPaint::point_getter2;
	virtual void drawImpl_(KvPaint*, point_getter2, unsigned nx, unsigned ny, unsigned ch) const;

private:

	// 该标记为真时，将强制用默认Z值替换原来的z值，可用来在3d空间绘制二维的colormap图
	bool forceDefaultZ_{ false }; 

	bool showBorder_{ false };
	KpPen borderPen_;
};
