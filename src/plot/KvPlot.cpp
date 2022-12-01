#include "KvPlot.h"
#include "KvCoord.h"
#include "KvPaint.h"
#include "layout/KcLayoutGrid.h"
#include "layout/KuLayoutHelper.h"


KvPlot::KvPlot(std::shared_ptr<KvPaint> paint, std::shared_ptr<KvCoord> coord)
	: paint_(paint)
	, coord_(coord)
{
	legend_ = std::make_unique<KcLegend>();

	layout_ = std::make_unique<KcLayoutGrid>();
	layout_->putAt(0, 0, coord_.get());
}


KvPlot::~KvPlot()
{
	KuLayoutHelper::take(legend_.get());
	KuLayoutHelper::take(coord_.get());
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

	auto rcCanvas = paint_->viewport();
	updateLayout_(rcCanvas, paint_.get()); // 在调用beginPaint之后更新布局

	// 计算legend的空间
	
	auto rcCoord = rcCanvas;
	int ali = legend_->location();
	point2d szLegend; // 需要时计算

	bool showLegend = showLegend_ && legend_->itemCount() > 0;
	//if (showLegend) {
	//	szLegend = legend_->calcSize(paint_.get());
	//	rcCoord = KuAlignment::layout(ali, szLegend, rcCanvas);
	//}

	coord().draw(paint_.get());

	auto rcPlot = coord().getPlotRect();
	paint_->setViewport(rcPlot); // plottable绘制需要设定plot视图，以便按世界坐标执行绘制操作
	paint_->pushClipRect(rcPlot); // 设置clipRect，防止plottables超出范围

	for (int idx = 0; idx < plottableCount(); idx++)
		if (plottableAt(idx)->visible())
		    plottableAt(idx)->draw(paint_.get());

	paint_->popClipRect();

	if (showLegend) 
//		auto pos = KuAlignment::position(ali, szLegend, KuAlignment::outside(ali) ? rcCoord : rcPlot);
		legend_->draw(paint_.get());

	paint_->setViewport(rcCanvas); // 恢复原视口

	paint_->endPaint();
}


void KvPlot::fitData()
{
	typename KvRenderable::aabb_t box;
	for (auto& p : plottables_)
		box.merge(p->boundingBox());

	if (!box.isNull())
		coord_->setExtents(box.lower(), box.upper());
	else
		coord_->setExtents({ 0, 0, 0 }, { 1, 1, 1 });
}


void KvPlot::updateLayout_(const rect_t& rc, void* cxt)
{
	KuLayoutHelper::take(legend_.get());

	bool showLegend = showLegend_ && legend_->itemCount() > 0;
	if (showLegend) {
		auto loc = legend_->location();
		//legend_->align() = loc;
		coord_->placeElement(legend_.get(), loc);
	}

	layout_->calcSize(cxt);
	layout_->arrange(rc); // 布局plot各元素
}
