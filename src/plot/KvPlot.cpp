#include "KvPlot.h"
#include "KvCoord.h"
#include "KvPaint.h"


KvPlot::KvPlot(std::shared_ptr<KvPaint> paint, std::shared_ptr<KvCoord> coord)
	: paint_(paint)
	, coord_(coord)
{
	legend_ = std::make_unique<KcLegend>();
}


KvPlottable* KvPlot::plottableAt(unsigned idx)
{
	assert(idx < plottableCount());
	return plottables_[idx].get();
}


void KvPlot::addPlottable(KvPlottable* plt)
{
	plottables_.emplace_back(plt);

	if (plt->majorColorsNeeded() != -1)
		legend_->addItem(plt);
}


void KvPlot::removePlottable(KvPlottable* plt)
{
	if (plt->majorColorsNeeded() != -1)
		legend_->removeItem(plt);

	for (auto iter = plottables_.cbegin(); iter != plottables_.cend(); iter++)
		if (iter->get() == plt) {
			plottables_.erase(iter);
			break;
		}
}


void KvPlot::setPlottableAt(unsigned idx, KvPlottable* plt)
{
	legend_->removeItem(plottables_[idx].get());
	if (plt->majorColorsNeeded() != -1)
		legend_->addItem(plt);

	assert(idx < plottableCount());
	return plottables_[idx].reset(plt);
}


void KvPlot::removePlottableAt(unsigned idx)
{
	legend_->removeItem(plottables_[idx].get());
	plottables_.erase(plottables_.begin() + idx);
}


void KvPlot::removeAllPlottables()
{
	legend_->clear();
	plottables_.clear();
}


void KvPlot::update()
{
	if (autoFit_ && !plottables_.empty())
		fitData();

	autoProject_();

	paint_->beginPaint();

	coord().draw(paint_.get());

	paint_->pushClipRect(paint_->viewport());

	for (int idx = 0; idx < plottableCount(); idx++)
		if (plottableAt(idx)->visible())
		    plottableAt(idx)->draw(paint_.get());

	paint_->pushCoord(KvPaint::k_screen);
	auto rc = paint_->viewport();
	paint_->pushLocal(KvPaint::mat4::buildTanslation({rc.lower().x(), rc.lower().y(), 0}));
	legend_->draw(paint_.get());
	paint_->popLocal();
	paint_->popCoord();

	paint_->popClipRect();

	paint_->endPaint();
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
