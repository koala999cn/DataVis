#include "KvPlot.h"


KvPlot::KvPlot() 
	: ortho_(true)
	, fitData_(true)
    , isometric_(false)
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
	if (fitData_ && !plottables_.empty()) 
		autoFit_();

	autoProject_();

	updateImpl_();
}


void KvPlot::autoFit_()
{
	KtAABB<double> box;
	for (auto& p : plottables_)
		box.merge(p->boundingBox());

	if(!box.isNull())
	    coord_->setExtents(box.lower(), box.upper());
}
