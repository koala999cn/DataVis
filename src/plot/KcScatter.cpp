#include "KcScatter.h"
#include "plot/KvPaint.h"


void KcScatter::drawImpl_(KvPaint* paint, point_getter getter, const color4f& majorColor) const
{
	auto cxt = scatCxt_;
	cxt.color = majorColor;
	paint->apply(cxt);
	paint->drawPoints(getter, data()->size());
}
