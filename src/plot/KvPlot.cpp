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

	auto axisSwapped = coord_->axisSwapped();
	if (axisSwapped)
		paint_->pushLocal(coord_->axisSwapMatrix());

	autoProject_();

	paint_->beginPaint();

	updateLayout_(paint_->viewport());

	paint_->setViewport(layout_->innerRect()); // 此处压入innerRect，与KcCoord3d配合抑制fixPlotView_修正plot3d的视口偏移
	                                           // TODO: 更优雅和通用的实现

	// 修正视口偏移（主要针对plot2d，把它的坐标系lower点移到视口的左下角）
	auto locals = fixPlotView_(); // 此处有locals个矩阵入栈，后续须pop

	coord_->draw(paint_.get());

	auto axisInversed = coord_->axisInversed();
	if (axisInversed)
		paint_->pushLocal(coord_->axisInverseMatrix());

	drawPlottables_();

	if (axisInversed)
		paint_->popLocal();

	if (axisSwapped)
		paint_->popLocal();

	for (int i = 0; i < locals; i++)
	    paint_->popLocal();

	if (realShowLegend_()) 
		legend_->draw(paint_.get());

	if (realShowColorBar_())
		colorBar_->draw(paint_.get());

	// debug drawing
	if (showLayoutRect_)
	    drawLayoutRect_();

	paint_->endPaint();
}


int KvPlot::fixPlotView_()
{
	auto rcCanvas = paint_->viewport();
	auto rcPlot = coord_->getPlotRect();
	if (rcPlot == rcCanvas)
		return 0;
		
	// 绘图区域相对于画布（窗口视图）的缩放比例
	KvPaint::point3 scale = { rcPlot.width() / rcCanvas.width(),
								rcPlot.height() / rcCanvas.height(), 1 };
	//if (coord_->axisSwapped() == KvCoord::k_axis_swap_xy)
	//	std::swap(scale.x(), scale.y());
	scale = paint_->localToWorldV(scale); // 等价于上述坐标轴交换代码，此处使用更通用的变换方法
	auto scaleMat = KvPaint::mat4::buildScale(scale);
	paint_->pushLocal(scaleMat);

	// 把世界坐标系的lower点偏移到rcPlot的左下点
	auto lower = paint_->unprojectp({ rcPlot.lower().x(), rcPlot.upper().y() });
	auto shiftMat = KvPaint::mat4::buildTanslation(lower - coord_->lower());
	
	/////////////////////////////////////////////////////////////////////////
	// 上述偏移等价于以下代码
	// 绘图区域相对于画布（窗口视图）的偏移，屏幕坐标下的像素值
	//KvPaint::point2 shift = { rcPlot.lower().x() - rcCanvas.lower().x(),
	//							rcPlot.upper().y() - rcCanvas.upper().y() };
	//auto shift3d = paint_->unprojectv(shift); // 转换到世界坐标
	// 此外，由于缩放变换是相对于原点进行的，这就造成了坐标系的lower点产生了偏移，需要进一步修正
	//shift3d += (coord_->lower() - coord_->lower() * scale);
	//auto shiftMat = KvPaint::mat4::buildTanslation(shift3d);
	//////////////////////////////////////////////////////////////////////////

	paint_->pushLocal(shiftMat);

	//assert(std::floor(paint_->projectp(coord_->lower()).x()) == rcPlot.lower().x());
	//assert(std::floor(paint_->projectp(coord_->lower()).y()) == rcPlot.upper().y());

	return 2;
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
			KuLayoutHelper::take(colorBar_);
			if (colorBar_) delete colorBar_;
			colorBar_ = nullptr;
		}
	}

	if (addedPlt) {
		if (addedPlt->majorColorsNeeded() != -1) {
			legend_->addItem(addedPlt);
		}
		else {
			assert(colorBar_ == nullptr); // TODO: 多通道数据
			if (colorBar_) {
				KuLayoutHelper::take(colorBar_);
				delete colorBar_;
			}
			colorBar_ = new KcColorBar(addedPlt);
		}
	}
}


void KvPlot::drawPlottables_()
{
	//paint_->pushClipRect(paint_->viewport()); // 设置clipRect，防止plottables超出范围

	for (int idx = 0; idx < plottableCount(); idx++)
		if (plottableAt(idx)->visible())
			plottableAt(idx)->draw(paint_.get());

	//paint_->popClipRect();
}


void KvPlot::setMargins(const margins_t& m)
{ 
	rect_t rc;
	rc.lower() = { m.left(), m.top() }; // TODO: 
	rc.upper() = { m.right(), m.bottom() };
	
	layout_->setMargins(rc); 
}


void KvPlot::setMargins(float l, float t, float r, float b)
{
	return setMargins({ l, t, r, b });
}


KvPlot::margins_t KvPlot::margins() const
{ 
	auto rc = layout_->margins();
	margins_t m;
	m.left() = rc.lower().x();
	m.right() = rc.upper().x();
	m.top() = rc.lower().y();
	m.bottom() = rc.upper().y();
	return m; 
}


#include <queue>
void KvPlot::drawLayoutRect_()
{
	std::queue<KvLayoutElement*> eles; // 待绘制布局元素
	eles.push(layout_.get());

	paint_->pushCoord(KvPaint::k_coord_screen);
	paint_->setColor({ 1,0,0,1 });
	
	while (!eles.empty()) {
		auto e = eles.front(); eles.pop();
		paint_->drawRect(e->outterRect());
		auto c = dynamic_cast<KvLayoutContainer*>(e);
		if (c) {
			for (auto& i : c->elements())
				if (i) eles.push(i.get());
		}
	}

	paint_->popCoord();
}
