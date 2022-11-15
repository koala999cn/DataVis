#include "KcLineFilled.h"
#include "KvPaint.h"
#include "KvData.h"
#include <assert.h>


void KcLineFilled::drawImpl_(KvPaint* paint, point_getter getter1, unsigned count, unsigned ch) const
{
	point_getter getter2 = [getter1](unsigned i) {
		auto pt = getter1(i);
		pt.y() = 0;
		return pt;
	};

	paint->apply(fillCxt_);

	if (count > 4096) { // TODO：使用降采样算法
		unsigned stride = count / 4096 + 1;
		paint->fillBetween([&getter1, stride](unsigned idx) {
			return getter1(stride * idx); }, getter2, count / stride);
	}
	else {
		paint->fillBetween(getter1, getter2, count);
	}
}


unsigned KcLineFilled::majorColorsNeeded() const
{
	return 1;
}


bool KcLineFilled::minorColorNeeded() const
{
	return true;
}


unsigned KcLineFilled::majorColors() const
{
	return 1;
}


color4f KcLineFilled::majorColor(unsigned idx) const
{
	return fillCxt_.color;
}


void KcLineFilled::setMajorColors(const std::vector<color4f>& majors)
{
	assert(majors.size() == 1);
	fillCxt_.color = majors.front();
}


color4f KcLineFilled::minorColor() const
{
	return lineCxt_.color;
}

void KcLineFilled::setMinorColor(const color4f& minor)
{
	lineCxt_.color = minor;
}
