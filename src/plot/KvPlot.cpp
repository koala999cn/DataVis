#include "KvPlot.h"
#include "KvCoord.h"
#include "KvPaint.h"
#include "KuAlignment.h"


KvPlot::KvPlot(std::shared_ptr<KvPaint> paint, std::shared_ptr<KvCoord> coord)
	: paint_(paint)
	, coord_(coord)
{
	legend_ = std::make_unique<KcLegend>();

	layMgr_.setRoot(coord_.get());
}


KvPlot::~KvPlot()
{
	layMgr_.take(coord_.get());
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
	updateLayout_(rcCanvas, paint_.get()); // �ڵ���beginPaint֮����²���

	// ����legend�Ŀռ�
	
	auto rcCoord = rcCanvas;
	int ali = legend_->alignment();
	point2d szLegend; // ��Ҫʱ����

	bool showLegend = showLegend_ && legend_->itemCount() > 0;
	if (showLegend) {
		szLegend = legend_->calcSize(paint_.get());
		rcCoord = KuAlignment::layout(ali, szLegend, rcCanvas);
	}

//	auto rcPlot = rcCoord; // ��ͼ����
//	auto mrgCoord = coord().calcMargins(paint_.get());
//	rcPlot.shrink({ mrgCoord.left(), mrgCoord.top() }, { mrgCoord.right(), mrgCoord.bottom() });
	// TOOD: ���rcPlot�Ƿ���

	coord().draw(paint_.get());

	auto rcPlot = coord().getPlotRect();
	paint_->setViewport(rcPlot); // plottable������Ҫ�趨plot��ͼ���Ա㰴��������ִ�л��Ʋ���
	paint_->pushClipRect(rcPlot); // ����clip����ֹplottables������Χ

	for (int idx = 0; idx < plottableCount(); idx++)
		if (plottableAt(idx)->visible())
		    plottableAt(idx)->draw(paint_.get());

	paint_->popClipRect();

	if (showLegend) {
//		auto pos = KuAlignment::position(ali, szLegend, KuAlignment::outside(ali) ? rcCoord : rcPlot);

		paint_->pushCoord(KvPaint::k_coord_screen);
//		paint_->pushLocal(KvPaint::mat4::buildTanslation({ pos.x(), pos.y(), 0 }));

		legend_->draw(paint_.get());

//		paint_->popLocal();
		paint_->popCoord();
	}

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
	layMgr_.root()->calcSize(cxt);
	layMgr_.root()->arrange(rc); // ����plot��Ԫ��
}
