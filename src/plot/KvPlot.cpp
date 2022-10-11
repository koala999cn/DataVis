#include "KvPlot.h"


KvPlot::KvPlot(std::shared_ptr<KvPaint3d> paint)
	: paint_(paint)
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
	for (int idx = 0; idx < plottableCount(); idx++)
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
