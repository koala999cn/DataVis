#include "KcGraph.h"


KtAABB<double> KcGraph::boundingBox() const
{
	KtAABB<double> box; // null
	auto& d = super_::data();
	if (d)
		for (auto& pt : *d) 
			box.merge(pt);

	return box;
}


void KcGraph::draw(KglPaint* paint) const
{
	if (visible() && !empty()) {
		paint->setColor(majorColor());
		paint->drawLineStrip(*data());
	}
}
