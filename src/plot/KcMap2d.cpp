#include "KcMap2d.h"


void KcMap2d::draw(KvPaint* paint) const
{
	if (empty())
		return;

	auto d = data();
	assert(d->dim() == 2);
}
