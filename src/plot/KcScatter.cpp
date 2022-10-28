#include "KcScatter.h"
#include "plot/KvPaint.h"


void KcScatter::drawImpl_(KvPaint* paint, point_getter getter, unsigned count, const color4f& majorColor) const
{
	auto cxt = scatBrush_;
	cxt.color = majorColor;
	paint->apply(cxt);
	paint->setPointSize(size_);
	paint->drawPoints(getter, count);
}
