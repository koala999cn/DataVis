#include "KvPlot3d.h"


KvPlot3d::KvPlot3d(std::shared_ptr<KvPaint> paint)
	: KvPlot(paint)
{
	coord_ = std::make_unique<KcCoordSystem>();
}


void KvPlot3d::update()
{
	if (autoFit_ && !plottables_.empty()) 
		fitData();

	autoProject_();

	coordSystem().draw(paint_.get());

	for (int idx = 0; idx < plottableCount(); idx++)
		plottable(idx)->draw(paint_.get());
}


void KvPlot3d::fitData()
{
	typename KvRenderable::aabb_type box;
	for (auto& p : plottables_)
		box.merge(p->boundingBox());

	if(!box.isNull())
	    coord_->setExtents(box.lower(), box.upper());
}
