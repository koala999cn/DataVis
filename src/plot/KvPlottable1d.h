#pragma once
#include "KvPlottable.h"
#include "KvPaint.h"


// 序列图的基类，主要处理串行数据，用于绘制折线图graph、散点图scatter、条状图bar等
// 兼容2d和3d模式

class KvPlottable1d : public KvPlottable
{
public:

	using KvPlottable::KvPlottable;

protected:

	void drawDiscreted_(KvPaint*, KvDiscreted*) const override;

	using point_getter1 = typename KvPaint::point_getter1;

	// 为了兼容连续数据，此处增加unsigned参数，表示数据count
	virtual void drawImpl_(KvPaint*, point_getter1, unsigned count, unsigned channels) const = 0;

private:

	// 一维数据(x, y)在x-y平面绘图（使用缺省z值）
	void draw1d_(KvPaint*, KvDiscreted*) const;

	// 二维数据(x, y, z)画瀑布图, 以x为流动轴
	void draw2d_(KvPaint*, KvDiscreted*) const;

	// 二维数据(x, y, z)画3d图
	void draw3d_(KvPaint*, KvDiscreted*) const;

};
