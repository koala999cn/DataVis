#pragma once
#include "KvPlottable.h"
#include "KvPaint.h"


// 序列图，包括折线图graph、散点图scatter、条状图bar等
// 兼容2d和3d模式

class KvPlottable1d : public KvPlottable
{
public:

	using KvPlottable::KvPlottable;

	void draw(KvPaint*) const override;

protected:

	using point_getter = typename KvPaint::point_getter;

	// 为了兼容连续数据，此处增加unsigned参数，表示数据count
	virtual void drawImpl_(KvPaint*, point_getter, unsigned, const color4f&) const = 0;

private:

	// 以下成员仅对二维数据有效

	float_t defaultZ_{ 0 }; // 二维数据的z轴将被置为该值
	float_t stepZ_{ 1 }; // 多通道二维数据的z轴偏移。若须将多通道数据显示在一个z平面，置该值为0
};
