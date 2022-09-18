#include "KvPlot.h"
#include "KvPlottable.h"
#include "KcCoordSystem.h"


KvPlot::KvPlot() 
	: ortho_(true)
	, fitData_(true)
{
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
	if (fitData_ && !plottables_.empty()) {
		auto box = coord_->boundingBox();
		autoFit_();
		if(box != coord_->boundingBox())
			autoProject_();
	}

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