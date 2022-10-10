#include "KvPlot.h"


KvPlot::KvPlot(std::shared_ptr<KvPaint> paint)
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


void KvPlot::removeAllPlottables()
{
	plottables_.clear();
}
