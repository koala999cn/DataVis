#include "KvPlot3d.h"


KvPlot3d::KvPlot3d(std::shared_ptr<KvPaint> paint)
	: KvPlot(paint)
{
	coord_ = std::make_unique<KcCoord3d>();
}


void KvPlot3d::update()
{
	if (autoFit_ && !plottables_.empty()) 
		fitData();

	autoProject_();

	coordSystem().draw(paint_.get());

	KvPlot::update(); // draw the plottables
}


void KvPlot3d::fitData()
{
	aabb_type box;
	for (auto& p : plottables_)
		box.merge(p->boundingBox());

	if (!box.isNull())
		coord_->setExtents(box.lower(), box.upper());
	else
		coord_->setExtents({ 0, 0, 0 }, { 1, 1, 1 });
}
