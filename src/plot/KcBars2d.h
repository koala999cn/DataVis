#pragma once
#include "KvPlottable1d.h"
#include "KpContext.h"


// 2维柱状图

class KcBars2d : public KvPlottable1d
{
	using super_ = KvPlottable1d;
	using super_::point_getter;

public:

	using super_::super_;

	float_t barWidthRatio() const { return barWidthRatio_; }
	float_t& barWidthRatio() { return barWidthRatio_; }

	aabb_type boundingBox() const;

protected:

	void drawImpl_(KvPaint*, point_getter, const color4f&) const override;

	// 返回单个bar的宽度（世界坐标）
	float_t barWidth_() const;

protected:
	KpPen border_;
	KpBrush fill_;
	float_t barWidthRatio_{ 0.5 };
	float_t baseLine_{ 0 }; // bar的底线
};
