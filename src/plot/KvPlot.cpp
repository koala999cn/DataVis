#include "KvPlot.h"


KvPlot::KvPlot(std::shared_ptr<KvPaint> paint)
	: ortho_(true)
	, autoFit_(true)
    , isometric_(false)
	, paint_(paint)
{
	zoom_ = 1;
	scale_ = { 1, 1, 1 };
	shift_ = { 0, 0, 0 };
	rotate_ = { 0, 0, 0 };

	coord_ = std::make_unique<KcCoordSystem>();
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
		fitData_();

	autoProject_();

	coordSystem().draw(*paint_);

	for (int idx = 0; idx < plottableCount(); idx++)
		plottable(idx)->draw(*paint_);
}


void KvPlot::fitData_()
{
	KtAABB<double> box;
	for (auto& p : plottables_)
		box.merge(p->boundingBox());

	if(!box.isNull())
	    coord_->setExtents(box.lower(), box.upper());
}
