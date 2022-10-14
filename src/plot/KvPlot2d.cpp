#include "KvPlot2d.h"
#include "KtAABB.h"


KvPlot2d::KvPlot2d(std::shared_ptr<KvPaint> paint)
	: KvPlot(paint)
{
	coord_ = std::make_unique<KcCoord2d>();
}


void KvPlot2d::update()
{
	if (autoFit_ && !plottables_.empty())
		fitData();

	coord().draw(paint_.get());

	KvPlot::update(); // draw the plottables
}


void KvPlot2d::fitData()
{
	KtAABB<float_t> box;
	for (auto& p : plottables_)
		box.merge(p->boundingBox());

	if (!box.isNull())
		coord_->setExtents({ box.lower().x(), box.lower().y() }, 
			{ box.upper().x(), box.upper().y() });
	else
		coord_->setExtents({ 0, 0 }, { 1, 1 });
}
