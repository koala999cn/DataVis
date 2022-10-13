#pragma once
#include "KvPlottable3d.h"
#include "KpContext.h"

// 3d’€œﬂÕº

class KcGraph3d : public KvPlottable3d
{
	using super_ = KvPlottable3d;
	using super_::point_getter;

public:

	using super_::super_;

	void drawImpl_(KvPaint*, point_getter, const color4f&) const override;

private:
	KpLineContext lineCxt_;
};
