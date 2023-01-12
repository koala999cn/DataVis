#pragma once
#include "KvPlottable2d.h"


// 绘制grid(sampled2d)数据的quads曲面

class KcSurface : public KvPlottable2d
{
	using super_ = KvPlottable2d;

public:
	using super_::super_;

private:
	void drawImpl_(KvPaint*, point_getter2, unsigned nx, unsigned ny, unsigned channels) const final;

private:
	bool flat_{ false }; // 是否使用flat模式渲染. 若true，每个quad使用同色渲染
	unsigned flatIdx_{ 0 }; // flat模式下，用于渲染quad的顶点序号
	                        // 即，对于每个quad，使用该quad的第flatIdx_顶点的颜色渲染整个quad
};
