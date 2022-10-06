#include "KvPlot3d.h"


KvPlot3d::KvPlot3d(std::shared_ptr<KvPaint> paint)
	: paint_(paint)
{
	coord_ = std::make_unique<KcCoordSystem>();
}


KvPlottable* KvPlot3d::plottable(unsigned idx)
{
	return plottables_[idx].get();
}


void KvPlot3d::addPlottable(KvPlottable* plot) 
{
	plottables_.emplace_back(plot);
}


void KvPlot3d::update()
{
	if (autoFit_ && !plottables_.empty()) 
		fitData_();

	autoProject_();

	coordSystem().draw(*paint_);

	for (int idx = 0; idx < plottableCount(); idx++)
		plottable(idx)->draw(*paint_);
}


void KvPlot3d::fitData_()
{
	typename KvRenderable::aabb_tyle box;
	for (auto& p : plottables_)
		box.merge(p->boundingBox());

	if(!box.isNull())
	    coord_->setExtents(box.lower(), box.upper());
}
