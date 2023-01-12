#include "KcGraph.h"
#include "KvPaint.h"
#include "KvData.h"
#include <assert.h>


void KcGraph::drawImpl_(KvPaint* paint, point_getter1 getter, unsigned count, unsigned ch) const
{
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


unsigned KcGraph::majorColorsNeeded() const
{
	return 1;
}


bool KcGraph::minorColorNeeded() const 
{ 
	return false; 
}


unsigned KcGraph::majorColors() const 
{
	return 1;
}


color4f KcGraph::majorColor(unsigned idx) const 
{ 
	return lineCxt_.color;
}


void KcGraph::setMajorColors(const std::vector<color4f>& majors) 
{ 
	assert(majors.size() == 1);
	lineCxt_.color = majors.front();
}


color4f KcGraph::minorColor() const 
{ 
	return color4f::invalid();
}

void KcGraph::setMinorColor(const color4f& minor) 
{ 
	assert(false);
}
