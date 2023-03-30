#pragma once
#include "KvPlottable2d.h"


// ����grid(sampled2d)���ݵ�quads����
class KcSurface : public KvPlottable2d
{
	using super_ = KvPlottable2d;

public:

	using super_::super_;

private:

	void* drawObject_(KvPaint*, unsigned) const override;

	void* drawSolid_(KvPaint*, unsigned) const;
	void* drawColor_(KvPaint*, unsigned) const;
};