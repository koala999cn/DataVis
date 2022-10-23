#include "KcGraph.h"
#include "KvPaint.h"


void KcGraph::drawImpl_(KvPaint* paint, point_getter getter, const color4f& majorColor) const
{
	auto cxt = lineCxt_;
	cxt.color = majorColor;
	paint->apply(cxt);

	if (data()->size() > 4096) { // TODO：使用降采样算法
		unsigned stride = data()->size() / 4096 + 1;
		paint->drawLineStrip([&getter, stride](unsigned idx) {
			return getter(stride * idx); }, data()->size() / stride);
	}
	else {
		paint->drawLineStrip(getter, data()->size());
	}
}
