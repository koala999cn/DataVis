#pragma once
#include "KvPlottable.h"
#include "KpContext.h"

// 3dœﬂÕº

class KcGraph3d : public KvPlottable
{
	using super_ = KvPlottable;

public:

	using super_::super_;

	void draw(KvPaint3d*) const override;

	unsigned majorColorsNeeded() const override;

	bool minorColorNeeded() const override;

	const color4f& majorColor(unsigned idx) const override;
	color4f& majorColor(unsigned idx) override;

	const color4f& minorColor() const override;
	color4f& minorColor() override;

private:
	KpLineContext cxt_;
};
