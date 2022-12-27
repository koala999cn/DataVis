#pragma once
#include "KcColorMap.h"


// 绘制sampled2d数据的3d曲面

class KcSurface : public KcColorMap
{
	using super_ = KcColorMap;

public:
	using super_::super_;


private:
	void drawDiscreted_(KvPaint*, KvDiscreted*) const final;
};
