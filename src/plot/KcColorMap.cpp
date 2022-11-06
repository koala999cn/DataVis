#include "KcColorMap.h"
#include "KvData.h"


void KcColorMap::draw(KvPaint* paint) const
{
	if (empty())
		return;

	auto d = data();
	assert(d->dim() == 2);
}


color4f KcColorMap::mapValueToColor_(float_t val) const
{
	auto factor = (val - mapLower_) / (mapUpper_ - mapLower_);
	return color4f();
}