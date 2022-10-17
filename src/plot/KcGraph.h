#pragma once
#include "KvPlottable1d.h"
#include "KpContext.h"

// ’€œﬂÕº

class KcGraph : public KvPlottable1d
{
	using super_ = KvPlottable1d;
	using super_::point_getter;

public:

	using super_::super_;

	void drawImpl_(KvPaint*, point_getter, const color4f&) const override;

private:
	KpPen lineCxt_;
};
