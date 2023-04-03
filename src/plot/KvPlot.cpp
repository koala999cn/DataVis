#include "KvPlot.h"
#include "KvCoord.h"
#include "KvPaint.h"
#include "KcLegend.h"
#include "KcColorBar.h"
#include "layout/KcLayoutGrid.h"
#include "layout/KuLayoutHelper.h"


KvPlot::KvPlot(std::shared_ptr<KvPaint> paint, std::shared_ptr<KvCoord> coord, char dim)
	: paint_(paint)
	, coord_(coord)
	, dim_(dim)
{
	legend_ = std::make_unique<KcLegend>();
	putAt(0, 0, coord_.get());
}


KvPlot::~KvPlot()
{
	unlayoutLegendAndColorbars_();
	KuLayoutHelper::take(coord_.get());
}


void KvPlot::unlayoutLegendAndColorbars_()
{
	for (auto& i : colorbars_)
		KuLayoutHelper::take(i.get());

	assert(legend_);
	KuLayoutHelper::take(legend_.get());
	legend_->clear();
}


KvPlottable* KvPlot::plottableAt(unsigned idx)
{
	assert(idx < plottableCount());
	return plottables_[idx].get();
}


void KvPlot::addPlottable(KvPlottable* plt)
{
	plottables_.emplace_back(plt);

	for (unsigned i = 0; i < dim_; i++)
		if (!plt->axis(i))
			plt->setAxis(i, coord_->defaultAxis(i));

	syncLegendAndColorbars_();
}


void KvPlot::removePlottable(KvPlottable* plt)
{
	for (auto iter = plottables_.cbegin(); iter != plottables_.cend(); iter++)
		if (iter->get() == plt) {
			plottables_.erase(iter);
			break;
		}

	syncLegendAndColorbars_();
}


void KvPlot::setPlottableAt(unsigned idx, KvPlottable* plt)
{
	assert(idx < plottableCount());
	plottables_[idx].reset(plt);

	syncLegendAndColorbars_();
}


void KvPlot::removePlottableAt(unsigned idx)
{
	assert(idx < plottableCount());
	plottables_.erase(plottables_.begin() + idx);

	syncLegendAndColorbars_();
}


void KvPlot::removeAllPlottables()
{
	plottables_.clear();

	syncLegendAndColorbars_();
}


void KvPlot::update()
{
	auto axisSwapped = coord_->axisSwapped();
	if (axisSwapped)
		paint_->pushLocal(coord_->axisSwapMatrix()); // ��ѹ�������ύ������autoProject_Ҫ��

	autoProject_();

	paint_->beginPaint();

	auto rc = paint_->viewport();
	paint_->pushCoord(KvPaint::k_coord_screen);
	paint_->apply(background());
	paint_->fillRect(rc);
	paint_->popCoord();

	updateLayout_(rc);
	if (innerRect().volume() == 0) { // ĳ��ά�ȵĲ��ֳߴ�Ϊ0��������
		paint_->endPaint();
		return;
	}

	paint_->setViewport(innerRect()); 

	// �����ӿ�ƫ�ƣ���Ҫ���plot2d������������ϵlower���Ƶ��ӿڵ����½ǣ�
	auto locals = fixPlotView_(); // �˴���locals��������ջ��������pop

	coord_->draw(paint_.get());

	if (coord_->axisInversed()) {
		paint_->pushLocal(coord_->axisInverseMatrix());
		++locals;
	}

	if (dim() == 3)
		paint_->enableClipBox(coord_->lower(), coord_->upper());

	drawPlottables_();

	if (dim() == 3)
		paint_->disableClipBox();

	for (int i = 0; i < locals + axisSwapped; i++)
	    paint_->popLocal();

	// draw legend
	if (showLegend_()) 
		legend_->draw(paint_.get());

	// draw colorbars
	paint_->setViewport(outterRect()); // ȡ���ڿ���У�����colorbar��������labelԽ���ڿ�
	for (auto& i : colorbars_)
		if (i->visible())
		    i->draw(paint_.get());

	// draw for debugging
	if (showLayoutRect_)
	    drawLayoutRect_();

	paint_->endPaint();
}


int KvPlot::fixPlotView_()
{
	if (dim() != 2) {
		assert(paint_->viewport() == coord_->getPlotRect());
		return 0; // ֻ��plot2d��������
	}

	auto rcCanvas = paint_->viewport();
	auto rcPlot = coord_->getPlotRect();
	if (rcPlot == rcCanvas)
		return 0;
		
	// ��ͼ��������ڻ�����������ͼ�������ű���
	// NB: ��rcPlot��ĳ��ά�ȳߴ�Ϊ0ʱ��������������Ϊ1
	// ������������Ϊ0ʱ��mvp���󽫲����棬���unproject����nanֵ��
	KvPaint::point3 scale = { rcPlot.width() == 0 ? 1 : rcPlot.width() / rcCanvas.width(),
		                      rcPlot.height() == 0 ? 1 : rcPlot.height() / rcCanvas.height(),
							  1 };

	//if (coord_->axisSwapped() == KvCoord::k_axis_swap_xy)
	//	std::swap(scale.x(), scale.y());
	scale = paint_->localToWorldV(scale); // �ȼ������������ύ�����룬�˴�ʹ�ø�ͨ�õı任����
	auto scaleMat = KvPaint::mat4::buildScale(scale);
	paint_->pushLocal(scaleMat);

	// ����������ϵ��lower��ƫ�Ƶ�rcPlot�����µ�
	auto lower = paint_->unprojectp({ rcPlot.lower().x(), rcPlot.upper().y() });
	lower.z() = coord_->lower().z(); // z�᲻�ƶ�
	auto shiftMat = KvPaint::mat4::buildTanslation(lower - coord_->lower());
	
	/////////////////////////////////////////////////////////////////////////
	// ����ƫ�Ƶȼ������´���
	// ��ͼ��������ڻ�����������ͼ����ƫ�ƣ���Ļ�����µ�����ֵ
	//KvPaint::point2 shift = { rcPlot.lower().x() - rcCanvas.lower().x(),
	//							rcPlot.upper().y() - rcCanvas.upper().y() };
	//auto shift3d = paint_->unprojectv(shift); // ת������������
	// ���⣬�������ű任�������ԭ����еģ�������������ϵ��lower�������ƫ�ƣ���Ҫ��һ������
	//shift3d += (coord_->lower() - coord_->lower() * scale);
	//auto shiftMat = KvPaint::mat4::buildTanslation(shift3d);
	//////////////////////////////////////////////////////////////////////////

	paint_->pushLocal(shiftMat);

	//assert(std::floor(paint_->projectp(coord_->lower()).x()) == rcPlot.lower().x());
	//assert(std::floor(paint_->projectp(coord_->lower()).y()) == rcPlot.upper().y());

	return 2;
}


namespace kPrivate
{
	static void fixInf(double& val)
	{
		// NB: ��ֵ̫��Ļ���axis���ƻᡰ�ɡ�
		constexpr typename KvRenderable::float_t maxV = 1e99; // std::numeric_limits<KvRenderable::float_t>::max() / 10.;
		constexpr typename KvRenderable::float_t minV = -1e99; // std::numeric_limits<KvRenderable::float_t>::lowest() / 10.;

		if (val == -KuMath::inf<KvRenderable::float_t>())
			val = minV;
		else if (val == KuMath::inf<KvRenderable::float_t>())
			val = maxV;
	}
}


void KvPlot::fitData()
{
	typename KvRenderable::aabb_t box;
	for (auto& p : plottables_)
		box.merge(p->boundingBox());

	if (box.isNull())
		box = { { 0, 0, 0 }, { 1, 1, 1 } };

	for (int i = 0; i < 3; i++) {
		assert(!std::isnan(box.lower()[i]) && !std::isnan(box.upper()[i]));

		kPrivate::fixInf(box.lower()[i]);
		kPrivate::fixInf(box.upper()[i]);

		if (box.lower()[i] == box.upper()[i]) {
			box.lower()[i] -= 1;
			box.upper()[i] += 1;
		}
	}

	coord_->setExtents(box.lower(), box.upper());
}


bool KvPlot::showLegend_() const
{
	return legend_->visible() && legend_->itemCount() > 0;
}


void KvPlot::updateLayout_(const rect_t& rc)
{
	syncLegendAndColorbars_();

	if (showLegend_()) {
		auto loc = legend_->location();
		legend_->align() = loc;
		coord_->placeElement(legend_.get(), loc);
	}

	for (auto& i : colorbars_) {
		if (i->visible()) {
			auto loc = i->location();
			i->align() = loc;
			coord_->placeElement(i.get(), loc);
		}
	}

	this->calcSize(paint_.get());
	this->arrange(rc); // ����plot��Ԫ��
}


void KvPlot::syncLegendAndColorbars_()
{
	assert(legend_);
	unlayoutLegendAndColorbars_();

	unsigned colorbarCount(0); // �¹�����colorbars����
	for (auto& plt : plottables_) {
		if (!plt->visible() || plt->empty())
			continue;

		if (plt->majorColorsNeeded() == -1) { // �����µ�colorbar
			if (colorbars_.size() < colorbarCount + 1) // NB: Ϊ�˱����û����õ�״̬������û�����colorbars_
				colorbars_.emplace_back(new KcColorBar(plt.get()));
			else
				colorbars_[colorbarCount]->resetPlottable(plt.get());
			++colorbarCount;
		}
		else { // ���legend��item
			legend_->addPlottable(plt.get());
		}
	}

	colorbars_.resize(colorbarCount);
}


void KvPlot::drawPlottables_()
{
	paint_->pushClipRect(coord_->getPlotRect()); // ����clipRect����ֹplottables������Χ

	for (int idx = 0; idx < plottableCount(); idx++)
		if (plottableAt(idx)->visible())
			plottableAt(idx)->draw(paint_.get());

	paint_->popClipRect();
}


#include <queue>
void KvPlot::drawLayoutRect_()
{
	std::queue<KvLayoutElement*> eles; // �����Ʋ���Ԫ��
	eles.push(this);

	paint_->pushCoord(KvPaint::k_coord_screen);
	
	while (!eles.empty()) {
		auto e = eles.front(); eles.pop();
		auto c = dynamic_cast<KvLayoutContainer*>(e);
		if (c) {
			for (auto i : c->elements())
				if (i) eles.push(i);
		}
		//else {
			paint_->setColor({ 0,0,1,1 }); // ��ɫ���ڿ�
			paint_->drawRect(e->innerRect());

			paint_->setColor({ 1,0,0,1 }); // ��ɫ�����
			paint_->drawRect(e->outterRect());
		//}
	}

	paint_->popCoord();
}
