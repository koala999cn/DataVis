#include "KcGraph.h"
#include "KvPaint.h"
#include "KvData.h"
#include <assert.h>


void KcGraph::drawImpl_(KvPaint* paint, point_getter1 getter, unsigned count, unsigned ch) const
{
	lineCxt_.color = majorColor(ch);
	paint->apply(lineCxt_);

	if (count > 4096) { // TODO：使用降采样算法
		unsigned stride = count / 4096 + 1;
		paint->drawLineStrip([&getter, stride](unsigned idx) {
			return getter(stride * idx); }, count / stride);
	}
	else {
		paint->drawLineStrip(getter, count);
	}
}


const color4f& KcGraph::minorColor() const
{ 
	static auto minor = color4f::invalid();
	return minor;
}


void KcGraph::setMinorColor(const color4f& minor) 
{ 
	// do nothing
}
