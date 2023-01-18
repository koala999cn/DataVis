#include "KcLineFilled.h"
#include "KvPaint.h"
#include "KvData.h"
#include <assert.h>


void KcLineFilled::drawImpl_(KvPaint* paint, GETTER getter, unsigned count, unsigned ch) const
{
	unsigned stride = count / 4096 + 1;
	auto downsamp = [getter, stride](unsigned idx) { // TODO：使用降采样算法
		return getter(stride * idx);
	};

	if (count > 4096)
		getter = downsamp, count /= stride;

	auto getter1 = toPaintGetter(getter, ch); // toPaintGetter按需完成z值替换

	auto getter2 = [getter1](unsigned i) {
		auto pt = getter1(i);
		pt[1] = 0;
		return pt;
	};

	fillCxt_.color = majorColor(ch);
	paint->apply(fillCxt_);
	paint->fillBetween(getter1, getter2, count);
}


const color4f& KcLineFilled::minorColor() const
{
	return lineCxt_.color;
}

void KcLineFilled::setMinorColor(const color4f& minor)
{
	lineCxt_.color = minor;
}
