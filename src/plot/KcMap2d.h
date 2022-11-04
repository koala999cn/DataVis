#pragma once
#include "KvPlottable.h"
#include "KpContext.h"


class KcMap2d : public KvPlottable
{
	using super_ = KvPlottable;

public:

	using super_::super_;

	unsigned majorColorsNeeded() const override { return -1; }

	bool minorColorNeeded() const override { return true; }

	void draw(KvPaint*) const override;
};