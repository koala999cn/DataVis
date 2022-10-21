#include "KcGraph.h"
#include "KvPaint.h"


void KcGraph::drawImpl_(KvPaint* paint, point_getter getter, const color4f& majorColor) const
{
	auto cxt = lineCxt_;
	cxt.color = majorColor;
	paint->apply(cxt);
    paint->drawLineStrip(getter, data()->size());
}
