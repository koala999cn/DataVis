#include "KcGraph3d.h"
#include "plot/KvPaint.h"
#include "KvDiscreted.h"


void KcGraph3d::drawImpl_(KvPaint* paint, point_getter getter, const color4f& majorColor) const
{
	auto cxt = lineCxt_;
	cxt.color = majorColor;
	paint->apply(cxt);
    paint->drawLineStrip(getter, data()->size());
}
