#pragma once
#include "plot/KvPlottable.h"


class KcImPlottable : public KvPlottable
{
public:
	using KvPlottable::KvPlottable;

	void draw(KvPaint*) const override;
};
