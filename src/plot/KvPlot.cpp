#include "KvPlot.h"
#include "KvCoord.h"


KvPlot::KvPlot(std::shared_ptr<KvPaint> paint, std::shared_ptr<KvCoord> coord)
	: paint_(paint)
	, coord_(coord)
{

}


KvPlottable* KvPlot::plottable(unsigned idx)
{
	return plottables_[idx].get();
}


void KvPlot::addPlottable(KvPlottable* plot)
{
	plottables_.emplace_back(plot);
}


void KvPlot::update()
{
	if (autoFit_ && !plottables_.empty())
		fitData();

	autoProject_();

	coord().draw(paint_.get());

	for (int idx = 0; idx < plottableCount(); idx++)
		if (plottable(idx)->visible())
		    plottable(idx)->draw(paint_.get());
}


void KvPlot::removePlottable(KvPlottable* plot)
{
	for (auto iter = plottables_.cbegin(); iter != plottables_.cend(); iter++)
		if (iter->get() == plot) {
			plottables_.erase(iter);
			break;
		}
}


void KvPlot::removePlottable(unsigned idx)
{
	plottables_.erase(plottables_.begin() + idx);
}


void KvPlot::removeAllPlottables()
{
	plottables_.clear();
}


void KvPlot::fitData()
{
	typename KvRenderable::aabb_type box;
	for (auto& p : plottables_)
		box.merge(p->boundingBox());

	if (!box.isNull())
		coord_->setExtents(box.lower(), box.upper());
	else
		coord_->setExtents({ 0, 0, 0 }, { 1, 1, 1 });
}
