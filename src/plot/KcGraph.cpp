#include "KcGraph.h"
#include "KvPaint.h"


void KcGraph::drawImpl_(KvPaint* paint, point_getter getter, unsigned count, const color4f& majorColor) const
{
	auto cxt = lineCxt_;
	cxt.color = majorColor;
	paint->apply(cxt);

	if (count > 4096) { // TODO：使用降采样算法
		unsigned stride = count / 4096 + 1;
		paint->drawLineStrip([&getter, stride](unsigned idx) {
			return getter(stride * idx); }, count / stride);
	}
	else {
		paint->drawLineStrip(getter, count);
	}
}
