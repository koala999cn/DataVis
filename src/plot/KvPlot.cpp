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

	autoProject_();

	paint_->beginPaint();

	auto rcCanvas = paint_->viewport();
	updateLayout_(rcCanvas, paint_.get()); // �ڵ���beginPaint֮����²���

	coord().draw(paint_.get());

	auto rcPlot = coord().getPlotRect();
	paint_->setViewport(rcPlot); // plottable������Ҫ�趨plot��ͼ���Ա㰴��������ִ�л��Ʋ���

	drawPlottables_();

	if (realShowLegend_()) 
		legend_->draw(paint_.get());

	if (realShowColorBar_())
		colorBar_->draw(paint_.get());

	paint_->setViewport(rcCanvas); // �ָ�ԭ�ӿ�

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

	layout_->calcSize(cxt);
	layout_->arrange(rc); // ����plot��Ԫ��
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
	paint_->pushClipRect(paint_->viewport()); // ����clipRect����ֹplottables������Χ

	// ���������ᷴת����
	bool inv(false);
	typename KvPaint::mat4 invMat = KvPaint::mat4::identity();
	if (coord().axisInversed(0)) {
		inv = true;
		invMat.m00() = -1; 
		invMat.m03() = coord().lower().x() + coord().upper().x();
	}
	if (coord().axisInversed(1)) {
		inv = true;
		invMat.m11() = -1;
		invMat.m13() = coord().lower().y() + coord().upper().y();
	}
	if (coord().axisInversed(2)) {
		inv = true;
		invMat.m22() = -1;
		invMat.m23() = coord().lower().z() + coord().upper().z();
	}

	if (inv) paint_->pushLocal(invMat); // TODO: ȷ��invMat���׸�local�任��

	for (int idx = 0; idx < plottableCount(); idx++)
		if (plottableAt(idx)->visible())
			plottableAt(idx)->draw(paint_.get());

	if (inv) paint_->popLocal();

	paint_->popClipRect();
}
