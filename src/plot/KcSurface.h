#pragma once
#include "KvPlottable2d.h"


// ����grid(sampled2d)���ݵ�quads����

class KcSurface : public KvPlottable2d
{
	using super_ = KvPlottable2d;

public:
	using super_::super_;

private:
	void drawImpl_(KvPaint*, point_getter2, unsigned nx, unsigned ny, unsigned channels) const final;
};
