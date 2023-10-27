#include "KvPlot.h"
#include "KvCoord.h"
#include "KvPaint.h"
#include "KcPlotTitle.h"
#include "KcLegend.h"
#include "KcColorBar.h"
#include "layout/KuLayoutHelper.h"
#include "layout/KcLayoutOverlay.h"


KvPlot::KvPlot(std::shared_ptr<KvPaint> paint, std::shared_ptr<KvCoord> coord, char dim)
	: paint_(paint)
	, coord_(coord)
	, dim_(dim)
	, KvDecorator(dim == 2 ? "Plot2d" : "Plot3d")
{
	showBkgnd() = true;
	showBorder() = false;

	title_ = std::make_unique<KcPlotTitle>(name());
	title_->location() = KeAlignment::k_top | KeAlignment::k_outter; // 设置标题缺省位置在plot的上方

	legend_ = std::make_unique<KcLegend>();
	auto olay = new KcLayoutOverlay();
	olay->append(coord_.get());
	putAt(0, 0, olay);
}


KvPlot::~KvPlot()
{
	unlayoutAllDecorators_();
	KuLayoutHelper::take(coord_.get());
}


void KvPlot::unlayoutAllDecorators_()
{
	for (auto& i : colorbars_)
		KuLayoutHelper::take(i.get());

	assert(legend_);
	KuLayoutHelper::take(legend_.get());
	legend_->clear();

	KuLayoutHelper::take(title_.get());
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


void KvPlot::update(KvPaint* paint)
{
	if (paint == nullptr)
		paint = paint_.get();

	auto axisSwapped = coord_->axisSwapped();
	if (axisSwapped)
		paint->pushLocal(coord_->axisSwapMatrix()); // 先压入坐标轴交换矩阵，autoProject_要用

	autoProject_();

	paint->beginPaint();

	auto rc = paint->viewport();

	updateLayout_(paint, rc);
	if (innerRect().volume() == 0) { // 某个维度的布局尺寸为0，不绘制
		paint->endPaint();
		return;
	}

	paint->pushCoord(KvPaint::k_coord_screen);
	//KvDecorator::draw(paint); // TODO:
	paint->apply(bkgndBrush());
	paint->fillRect(rc);
	paint->popCoord();

	paint->setViewport(innerRect()); 

	// 修正视口偏移（主要针对plot2d，把它的坐标系lower点移到视口的左下角）
	auto locals = fixPlotView_(paint); // 此处有locals个矩阵入栈，后续须pop

	coord_->draw(paint);

	if (coord_->axisInversed()) {
		paint->pushLocal(coord_->axisInverseMatrix());
		++locals;
	}

	if (dim() == 3)
		paint->enableClipBox(coord_->lower(), coord_->upper());

	drawPlottables_(paint);

	if (dim() == 3)
		paint->disableClipBox();

	for (int i = 0; i < locals + axisSwapped; i++)
	    paint->popLocal();

	// draw legend
	if (showLegend_()) 
		legend_->draw(paint);

	// draw colorbars
	paint->setViewport(outterRect()); // 取消内框剪切，允许colorbar的坐标轴label越出内框
	for (auto& i : colorbars_)
		if (i->visible())
		    i->draw(paint);
	
	if (title_->visible()) {
		paint->pushCoord(KvPaint::k_coord_screen); // TODO: 让title自行决定绘制坐标系
		title_->draw(paint);
		paint->popCoord();
	}
	

	// draw for debugging
	if (showLayoutRect_)
	    drawLayoutRect_(paint);

	paint->endPaint();
}


int KvPlot::fixPlotView_(KvPaint* paint)
{
	if (dim() != 2) {
		assert(paint->viewport() == coord_->getFrame()->innerRect());
		return 0; // 只对plot2d进行修正
	}

	auto rcCanvas = paint->viewport();
	auto rcPlot = coord_->getFrame()->innerRect();
	if (rcPlot == rcCanvas)
		return 0;
		
	// 绘图区域相对于画布（窗口视图）的缩放比例
	// NB: 当rcPlot的某个维度尺寸为0时，设置缩放因子为1
	// （当缩放因子为0时，mvp矩阵将不可逆，造成unproject返回nan值）
	KvPaint::point3 scale = { rcPlot.width() == 0 ? 1 : rcPlot.width() / rcCanvas.width(),
		                      rcPlot.height() == 0 ? 1 : rcPlot.height() / rcCanvas.height(),
							  1 };

	//if (coord_->axisSwapped() == KvCoord::k_axis_swap_xy)
	//	std::swap(scale.x(), scale.y());
	scale = paint->localToWorldV(scale); // 等价于上述坐标轴交换代码，此处使用更通用的变换方法
	auto scaleMat = KvPaint::mat4::buildScale(scale);
	paint->pushLocal(scaleMat);

	// 把世界坐标系的lower点偏移到rcPlot的左下点
	auto lower = paint->unprojectp({ rcPlot.lower().x(), rcPlot.upper().y() });
	lower.z() = coord_->lower().z(); // z轴不移动
	auto shiftMat = KvPaint::mat4::buildTanslation(lower - coord_->lower());
	
	/////////////////////////////////////////////////////////////////////////
	// 上述偏移等价于以下代码
	// 绘图区域相对于画布（窗口视图）的偏移，屏幕坐标下的像素值
	//KvPaint::point2 shift = { rcPlot.lower().x() - rcCanvas.lower().x(),
	//							rcPlot.upper().y() - rcCanvas.upper().y() };
	//auto shift3d = paint->unprojectv(shift); // 转换到世界坐标
	// 此外，由于缩放变换是相对于原点进行的，这就造成了坐标系的lower点产生了偏移，需要进一步修正
	//shift3d += (coord_->lower() - coord_->lower() * scale);
	//auto shiftMat = KvPaint::mat4::buildTanslation(shift3d);
	//////////////////////////////////////////////////////////////////////////

	paint->pushLocal(shiftMat);

	//assert(std::floor(paint->projectp(coord_->lower()).x()) == rcPlot.lower().x());
	//assert(std::floor(paint->projectp(coord_->lower()).y()) == rcPlot.upper().y());

	return 2;
}


namespace kPrivate
{
	static void fixInf(double& val)
	{
		// NB: 数值太大的话，axis绘制会“飞”
		constexpr double maxV = 1e99; // std::numeric_limits<KvRenderable::float_t>::max() / 10.;
		constexpr double minV = -1e99; // std::numeric_limits<KvRenderable::float_t>::lowest() / 10.;

		if (val == -KuMath::inf<double>())
			val = minV;
		else if (val == KuMath::inf<double>())
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

		// TODO: 更好的处理inf的方式
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


void KvPlot::updateLayout_(KvPaint* paint, const rect_t& rc)
{
	unlayoutAllDecorators_();
	syncLegendAndColorbars_();

	if (showLegend_()) 
		layoutDecorator_(legend_.get());

	for (auto& i : colorbars_) {
		if (i->visible()) 
			layoutDecorator_(i.get());
	}

	// 最后放置title，确保title在外侧
	if (title_->visible())
		layoutDecorator_(title_.get());

	this->calcSize(paint);
	this->arrange(rc); // 布局plot各元素
}


void KvPlot::layoutDecorator_(KvDecoratorAligned* deco)
{
	deco->align() = deco->location(); // TODO: 优化此设计

	auto frame = (deco->alignTarget() == KvDecoratorAligned::k_plot_frame) ?
		coord_->parent() : coord_->getFrame();
	auto olay = dynamic_cast<KcLayoutOverlay*>(frame);

	assert(olay);
	KuLayoutHelper::align(olay, deco);
}


void KvPlot::syncLegendAndColorbars_()
{
	assert(legend_);

	unsigned colorbarCount(0); // 新构建的colorbars数量
	for (auto& plt : plottables_) {
		if (!plt->visible() || plt->empty())
			continue;

		if (plt->majorColorsNeeded() == -1) { // 构建新的colorbar
			if (colorbars_.size() < colorbarCount + 1) // NB: 为了保存用户设置的状态，所以没有清空colorbars_
				colorbars_.emplace_back(new KcColorBar(plt.get()));
			else
				colorbars_[colorbarCount]->resetPlottable(plt.get());
			++colorbarCount;
		}
		else { // 添加legend的item
			legend_->addPlottable(plt.get());
		}
	}

	colorbars_.resize(colorbarCount);
}


void KvPlot::drawPlottables_(KvPaint* paint)
{
	paint->pushClipRect(coord_->innerRect()); // 设置clipRect，防止plottables超出范围

	for (int idx = 0; idx < plottableCount(); idx++) {
		auto plt = plottableAt(idx);
		if (plt->visible()) {
			vec3d scale(1);
			for (int i = 0; i < 2; i++) {
				if (!plt->axis(i)->main())  // 处理分离坐标轴
					scale[i] = coord_->defaultAxis(i)->length() / plt->axis(i)->length();
			}

			if (scale != vec3d(1)) {
				auto mat = KvPaint::mat4::buildScale(scale);
				mat = mat * KvPaint::mat4::buildTanslation({ -plt->axis(0)->lower(), -plt->axis(1)->lower(), 0 });
				mat = KvPaint::mat4::buildTanslation({ coord_->defaultAxis(0)->lower(), coord_->defaultAxis(1)->lower(), 0 }) * mat;
				paint->pushLocal(mat);
			}

			plt->draw(paint);

			if (scale != vec3d(1))
				paint->popLocal();
		}
	}

	paint->popClipRect();
}


#include <queue>
void KvPlot::drawLayoutRect_(KvPaint* paint)
{
	std::queue<KvLayoutElement*> eles; // 待绘制布局元素
	eles.push(this);

	paint->pushCoord(KvPaint::k_coord_screen);
	
	while (!eles.empty()) {
		auto e = eles.front(); eles.pop();
		auto c = dynamic_cast<KvLayoutContainer*>(e);
		if (c) {
			for (auto i : c->elements())
				if (i) eles.push(i);
		}
		//else {
			paint->setColor({ 0,0,1,1 }); // 蓝色画内框
			paint->drawRect(e->innerRect());

			paint->setColor({ 1,0,0,1 }); // 红色画外框
			paint->drawRect(e->outterRect());
		//}
	}

	paint->popCoord();
}
