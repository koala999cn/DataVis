#pragma once
#include "KvPlottable.h"
#include "KvPaint.h"


class KvPlottable3d : public KvPlottable
{
public:

	using KvPlottable::KvPlottable;

	void draw(KvPaint*) const override;

protected:

	using point_getter = typename KvPaint::point_getter;
	virtual void drawImpl_(KvPaint*, point_getter, const color4f&) const = 0;

private:
	float_t defaultZ_{ 0 }; // 二维数据的z轴将被置为该值
	float_t stepZ_{ 1 }; // 多通道二维数据的z轴偏移。若须将多通道数据显示在一个z平面，置该值为0
};
