#pragma once
#include "KvPlottable1d.h"

// µ„œﬂÕº

class KcGraph : public KvPlottable1d
{
	using super_ = KvPlottable1d;

public:

	enum KeType
	{
		k_polyline,
		k_scatter,
		k_line_fill
	};

	using super_::super_;

	KtAABB<double> boundingBox() const override;

	void draw(KvPaint&) const override;

private:
	KeType type_;
};
