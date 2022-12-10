#include "KvPlot.h"
#include "KvCoord.h"
#include "KvPaint.h"
#include "KcLegend.h"
#include "KcColorBar.h"
#include "layout/KcLayoutGrid.h"
#include "layout/KuLayoutHelper.h"


KvPlot::KvPlot(std::shared_ptr<KvPaint> paint, std::shared_ptr<KvCoord> coord)
	: paint_(paint)
	, coord_(coord)
{
	legend_ = new KcLegend;
	colorBar_ = nullptr;

	layout_ = std::make_unique<KcLayoutGrid>();
	layout_->putAt(0, 0, coord_.get());
}


KvPlot::~KvPlot()
{
	if (colorBar_)
		KuLayoutHelper::take(colorBar_);

	assert(legend_);
	KuLayoutHelper::take(legend_);
	KuLayoutHelper::take(coord_.get());

	delete legend_;
	delete colorBar_;
}


KvPlottable* KvPlot::plottableAt(unsigned idx)
{
	assert(idx < plottableCount());
	return plottables_[idx].get();
}


void KvPlot::addPlottable(KvPlottable* plt)
{
	plottables_.emplace_back(plt);
	syncLegendAndColorBar_(nullptr, plt);
}


void KvPlot::removePlottable(KvPlottable* plt)
{
	syncLegendAndColorBar_(plt, nullptr);

	for (auto iter = plottables_.cbegin(); iter != plottables_.cend(); iter++)
		if (iter->get() == plt) {
			plottables_.erase(iter);
			break;
		}
}


void KvPlot::setPlottableAt(unsigned idx, KvPlottable* plt)
{
	assert(idx < plottableCount());

	syncLegendAndColorBar_(plottables_[idx].get(), plt);

	return plottables_[idx].reset(plt);
}


void KvPlot::removePlottableAt(unsigned idx)
{
	assert(idx < plottableCount());

	syncLegendAndColorBar_(plottables_[idx].get(), nullptr);

	plottables_.erase(plottables_.begin() + idx);
}


void KvPlot::removeAllPlottables()
{
	assert(legend_);
	if (colorBar_) {
		delete colorBar_;
		colorBar_ = nullptr;
	}

	legend_->clear();
	plottables_.clear();
}


void KvPlot::update()
{
	if (autoFit_ && !plottables_.empty())
		fitData();

	//paint_->pushLocal(coord().localMatrix()); // 坐标轴的反转和交换矩阵，autoProject_需要用到

	auto axisSwapped = coord_->axisSwapped();
	if (axisSwapped)
		paint_->pushLocal(coord_->axisSwapMatrix());

	autoProject_();

	paint_->beginPaint();

	auto rcCanvas = paint_->viewport();
	updateLayout_(rcCanvas);

	coord().draw(paint_.get());
	
	auto rcPlot = coord().getPlotRect();
	paint_->setViewport(rcPlot); // plottable绘制需要设定plot视图，以便按世界坐标执行绘制操作

	auto axisInversed = coord_->axisInversed();
	if (axisInversed)
		paint_->pushLocal(coord_->axisInverseMatrix());

	drawPlottables_();

	if (axisInversed)
		paint_->popLocal();

	if (axisSwapped)
		paint_->popLocal();

	//paint_->popLocal(); // 后续绘制基于屏幕坐标，不再反转和交换坐标，弹出变换矩阵

	if (realShowLegend_()) 
		legend_->draw(paint_.get());

	if (realShowColorBar_())
		colorBar_->draw(paint_.get());

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


void KvPlot::updateLayout_(const rect_t& rc)
{
	assert(legend_);
	KuLayoutHelper::take(legend_);
	if (colorBar_)
	    KuLayoutHelper::take(colorBar_);

	if (realShowLegend_()) {
		auto loc = legend_->location();
		legend_->align() = loc;
		coord_->placeElement(legend_, loc);
	}

	if (realShowColorBar_()) {
		assert(colorBar_);
		auto loc = colorBar_->location();
		colorBar_->align() = loc;
		coord_->placeElement(colorBar_, loc);
	}

	layout_->calcSize(paint_.get());
	layout_->arrange(rc); // 布局plot各元素
}


bool KvPlot::realShowLegend_() const
{
	return showLegend_ && legend_ && legend_->itemCount() > 0;
}


bool KvPlot::realShowColorBar_() const
{
	return showColorBar_ && colorBar_;
}


void KvPlot::syncLegendAndColorBar_(KvPlottable* removedPlt, KvPlottable* addedPlt)
{
	assert(legend_);

	if (removedPlt) {
		if (removedPlt->majorColorsNeeded() != -1) {
			legend_->removeItem(removedPlt);
		}
		else {
			assert(colorBar_);
			if (colorBar_) delete colorBar_;
			colorBar_ = nullptr;
		}
	}

	if (addedPlt) {
		if (addedPlt->majorColorsNeeded() != -1) {
			legend_->addItem(addedPlt);
		}
		else {
			assert(colorBar_ == nullptr);
			colorBar_ = new KcColorBar(addedPlt);
		}
	}
}


void KvPlot::drawPlottables_()
{
	paint_->pushClipRect(paint_->viewport()); // 设置clipRect，防止plottables超出范围
	//paint_->pushLocal(coord().localMatrix());

	for (int idx = 0; idx < plottableCount(); idx++)
		if (plottableAt(idx)->visible())
			plottableAt(idx)->draw(paint_.get());

	//paint_->popLocal();
	paint_->popClipRect();
}
