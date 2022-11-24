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


KvPlot::rect KvPlot::calcCoordLayout_(const rect& rcPlot)
{
	auto rcCoord = rcPlot;
		
	int align = legend_->alignment();
	auto size = legend_->calcSize(paint_.get());

	if (align & k_align_horz_first) {
		if (align & k_align_left)
			rcCoord.lower().x() += size.x();
		else if (align & k_align_right)
			rcCoord.upper().x() -= size.x();
		else if (align & k_align_top)
			rcCoord.lower().y() += size.y();
		else if (align & k_align_bottom)
			rcCoord.upper().y() -= size.y();
	}
	else if (align & k_align_vert_first) {
		if (align & k_align_top)
			rcCoord.lower().y() += size.y();
		else if (align & k_align_bottom)
			rcCoord.upper().y() -= size.y();
		else if (align & k_align_left)
			rcCoord.lower().x() += size.x();
		else if (align & k_align_right)
			rcCoord.upper().x() -= size.x();
	}

	return rcCoord;
}


void KvPlot::update()
{
	if (autoFit_ && !plottables_.empty())
		fitData();

	autoProject_();

	paint_->beginPaint();

	// ����legend�Ŀռ�
	auto rcCanvas = paint_->viewport();
	auto rcCoord = rcCanvas;

	bool showLegend = showLegend_ && legend_->itemCount() > 0;
	if (showLegend) 
		rcCoord = calcCoordLayout_(rcCanvas);

	auto rcPlot = rcCoord; // ��ͼ����
	auto mrgCoord = coord().calcMargins(paint_.get());
	rcPlot.shrink({ mrgCoord.left(), mrgCoord.top() }, { mrgCoord.right(), mrgCoord.bottom() });
	// TOOD: ���rcPlot�Ƿ���

	paint_->setViewport(rcPlot); // coord������Ҫ�趨plot��ͼ���Ա㰴�������������Ʋ���

	coord().draw(paint_.get());

	paint_->pushClipRect(rcPlot); // ����clip����ֹplottables������Χ

	for (int idx = 0; idx < plottableCount(); idx++)
		if (plottableAt(idx)->visible())
		    plottableAt(idx)->draw(paint_.get());

	paint_->popClipRect();

	if (showLegend) {
		auto pos = legend_->location(paint_.get(), legend_->outter() ? rcCoord : rcPlot);

		paint_->pushCoord(KvPaint::k_coord_screen);
		paint_->pushLocal(KvPaint::mat4::buildTanslation({ pos.x(), pos.y(), 0 }));

		legend_->draw(paint_.get());

		paint_->popLocal();
		paint_->popCoord();
	}

	paint_->setViewport(rcCanvas); // �ָ�ԭ�ӿ�

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
