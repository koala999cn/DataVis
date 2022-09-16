#pragma once
#include "KtPlottable1d.h"
#include "KglPaint.h"


// µ„œﬂÕº

template<int DIM>
class KtGraph : public KtPlottable1d
{
	using super_ = KtPlottable1d<DIM>;

public:
	using super_::super_;

	void draw(KglPaint* paint) const override {
		paint->drawLineStrip();
	}

private:

};

