#include "KcThemedQCP.h"
#include <QPen>
#include <QFont>
#include <QColor>
#include "QCustomPlot/qcustomplot.h"


namespace kPrivate
{
	void for_axis(QCustomPlot* plot, int level, std::function<void(QCPAxis*)> op)
	{
		int filter(0);
		if (level & KvThemedPlot::k_left)
			filter |= QCPAxis::atLeft;
		if (level & KvThemedPlot::k_right)
			filter |= QCPAxis::atRight;
		if (level & KvThemedPlot::k_top)
			filter |= QCPAxis::atTop;
		if (level & KvThemedPlot::k_bottom)
			filter |= QCPAxis::atBottom;

		for (auto rect : plot->axisRects())
			for (auto axis : rect->axes())
				if (axis->axisType() & filter)
					op(axis);
	}

	void for_layout_element(QCustomPlot* plot, std::function<void(QCPLayoutElement*)> op)
	{
		auto grid = plot->plotLayout();
		auto elems = grid->elements(true);
		for (auto e : elems) op(e);
	}

	// 在for_layout_element基础上提供过滤功能
	void for_text_element(QCustomPlot* plot, int level, std::function<void(QCPLayoutElement*)> op)
	{
		for_layout_element(plot, [level, op](QCPLayoutElement* ele) {
			if (dynamic_cast<QCPLegend*>(ele) && !(level & KvThemedPlot::k_legend_title))
				return;

			if (dynamic_cast<QCPPlottableLegendItem*>(ele) && !(level & KvThemedPlot::k_legend_label))
				return;

			if (dynamic_cast<QCPTextElement*>(ele) && !(level & KvThemedPlot::k_plot_text))
				return;

			if (dynamic_cast<QCPColorScale*>(ele) && !(level & KvThemedPlot::k_legend_label))
				return;

			if (dynamic_cast<QCPAxisRect*>(ele) && !(level & KvThemedPlot::k_axis_text))
				return;

			op(ele);
			});
	}
}


KcThemedQCP::KcThemedQCP(std::shared_ptr<QCustomPlot> qcp)
{
	qcp_ = qcp;
	bkgnd_ = QColor("white");
	qcp->setBackground(bkgnd_); // 同步背景色

	legendAlign = k_align_right | k_align_top | k_align_horz_first;
}


KcThemedQCP::~KcThemedQCP()
{

}


QBrush KcThemedQCP::fill(int level) const
{
	if (level == k_plot) 
		return bkgnd_;
	else if (level == k_axis) 
		return qcp_->axisRect()->backgroundBrush();
	else if (level == k_legend) 
		return qcp_->legend->brush();
	else 
		assert(false);

	return Qt::NoBrush; // make compiler happy
}


void KcThemedQCP::applyFill(int level, const QBrush& brush)
{
	if (level == k_plot) {
		bkgnd_ = brush;
		qcp_->setBackground(bkgnd_);
	}
	else if (level == k_axis) {
		for (auto rect : qcp_->axisRects())
			rect->setBackground(brush);
	}
	else if (level == k_legend) {
		qcp_->legend->setBrush(brush);
	}
	else {
		assert(false);
	}
}


QPen KcThemedQCP::border(int level) const
{
	if (level == k_legend) 
	    return qcp_->legend->borderPen();
	
	return QPen(Qt::NoPen); // TODO: 其他元素的边框支持
}


void KcThemedQCP::applyBorder(int level, const QPen& pen)
{
	if (level == k_legend)
		qcp_->legend->setBorderPen(pen);

	// TODO: legend-icon's border
}


QMargins KcThemedQCP::margins(int level) const
{
	if (level == k_plot) 
		return qcp_->plotLayout()->minimumMargins();
	else if (level == k_axis) 
		return qcp_->axisRect()->minimumMargins();
	else if (level == k_legend) 
		return qcp_->legend->minimumMargins();
	else 
		assert(false);

	return { 0, 0, 0, 0 }; // make compiler happy
}


void KcThemedQCP::applyMargins(int level, const QMargins& margins)
{
	if (level == k_plot)
		qcp_->plotLayout()->setMinimumMargins(margins);
	else if (level == k_axis)
		qcp_->axisRect()->setMinimumMargins(margins);
	else if (level == k_legend)
		qcp_->legend->setMinimumMargins(margins);
	else
		assert(false);
}


bool KcThemedQCP::visible(int level) const
{
	if (level == k_legend)
		return legendVisible();

	assert(false); // TODO:
	return false;
}


void KcThemedQCP::applyVisible(int level, bool b)
{
	if (level & k_legend)
		setLegendVisible(b); // TODO: 区分legend元素

	if (level & (k_axis_all | k_grid_all))
		applyAxisVisible_(level & (k_axis_all | k_grid_all), b);
}


void KcThemedQCP::applyAxisVisible_(int level, bool b)
{
	bool wholeAxis = level == (k_axis_all | k_grid_all);
	bool showAxisRect = b || !wholeAxis;
	for (auto ar : qcp_->axisRects())
		ar->setVisible(showAxisRect);
	if (!showAxisRect)
		return;

	kPrivate::for_axis(qcp_.get(), level, [level, b](QCPAxis* axis) {

		std::function<QPen(const QPen&)> show = [](const QPen& pen) {
			QPen newPen(pen);
			if (newPen.style() == Qt::NoPen)
				newPen.setStyle(Qt::SolidLine);
			return newPen;
		};

		std::function<QPen(const QPen&)> hide = [](const QPen& pen) {
			QPen newPen(pen);
			newPen.setStyle(Qt::NoPen);
			return newPen;
		};

		auto op = b ? show : hide;

		if (level & k_axis_baseline)
			axis->setBasePen(op(axis->basePen()));
		if (level & k_axis_tick_major)
			axis->setTickPen(op(axis->tickPen()));
		if (level & k_axis_tick_minor)
			axis->setSubTickPen(op(axis->subTickPen()));

		auto grid = axis->grid();
		//if ((level & k_grid_line) == k_grid_line)
		//	grid->setVisible(b);
		if (level & k_grid_major)
			grid->setPen(op(grid->pen()));
		if (level & k_grid_minor) {
			grid->setSubGridVisible(b);
			if (b) {
				QPen pen = grid->subGridPen();
				if (pen.style() == Qt::NoPen) {
					pen.setStyle(Qt::DashDotLine);
					grid->setSubGridPen(pen);
				}
			}
		}
		if (level & k_grid_zeroline)
			grid->setZeroLinePen(op(grid->zeroLinePen()));

		if (level & k_label)
			axis->setTickLabels(b);

		if (level & k_axis_title) {
			// TODO: axis->setLabel
		}

		});
}


void KcThemedQCP::applyLine(int level, std::function<QPen(const QPen&)> op)
{
	kPrivate::for_axis(qcp_.get(), level, [level, op](QCPAxis* axis) {

		if (level & k_axis_baseline)
			axis->setBasePen(op(axis->basePen()));
		if (level & k_axis_tick_major)
			axis->setTickPen(op(axis->tickPen()));
		if (level & k_axis_tick_minor)
			axis->setSubTickPen(op(axis->subTickPen()));

		auto grid = axis->grid();
		if (level & k_grid_major)
			grid->setPen(op(grid->pen()));
		if (level & k_grid_minor)
			grid->setSubGridPen(op(grid->subGridPen()));
		if (level & k_grid_zeroline)
			grid->setZeroLinePen(op(grid->zeroLinePen()));
		});
}


void KcThemedQCP::applyText(int level, std::function<QFont(const QFont&)> op)
{
	qcp_->setFont(op(qcp_->font()));

	kPrivate::for_text_element(qcp_.get(), level, [level, op](QCPLayoutElement* ele) {
		if (dynamic_cast<QCPLegend*>(ele)) {
			auto legend = dynamic_cast<QCPLegend*>(ele);
			legend->setFont(op(legend->font()));
		}
		else if (dynamic_cast<QCPPlottableLegendItem*>(ele)) {
			auto item = dynamic_cast<QCPPlottableLegendItem*>(ele);
			item->setFont(op(item->font()));
		}
		else if (dynamic_cast<QCPTextElement*>(ele)) {
			auto text = dynamic_cast<QCPTextElement*>(ele);
			text->setFont(op(text->font()));
		}
		else if (dynamic_cast<QCPColorScale*>(ele)) {
			auto scale = dynamic_cast<QCPColorScale*>(ele);
			auto axis = scale->axis();
			if (axis) axis->setLabelFont(op(axis->labelFont()));
		}
		else if (dynamic_cast<QCPAxisRect*>(ele)) {
			// 后面单独处理
		}
	});

	kPrivate::for_axis(qcp_.get(), level, [level, op](QCPAxis* axis) {
		if (level & k_axis_title)
			axis->setLabelFont(op(axis->labelFont()));
		if (level & k_axis_label)
			axis->setTickLabelFont(op(axis->tickLabelFont()));
	});
}


void KcThemedQCP::applyTextColor(int level, std::function<QColor(const QColor&)> op)
{
	kPrivate::for_text_element(qcp_.get(), level, [level, op](QCPLayoutElement* ele) {
		if (dynamic_cast<QCPLegend*>(ele)) {
			auto legend = dynamic_cast<QCPLegend*>(ele);
			legend->setTextColor(op(legend->textColor()));
		}
		else if (dynamic_cast<QCPPlottableLegendItem*>(ele)) {
			auto item = dynamic_cast<QCPPlottableLegendItem*>(ele);
			item->setTextColor(op(item->textColor()));
		}
		else if (dynamic_cast<QCPTextElement*>(ele)) {
			auto text = dynamic_cast<QCPTextElement*>(ele);
			text->setTextColor(op(text->textColor()));
		}
		else if (dynamic_cast<QCPColorScale*>(ele)) {
			auto scale = dynamic_cast<QCPColorScale*>(ele);
			auto axis = scale->axis();
			if (axis) axis->setLabelColor(op(axis->labelColor()));
		}
		else if (dynamic_cast<QCPAxisRect*>(ele)) {
			// 后面单独处理
		}
	});

	kPrivate::for_axis(qcp_.get(), level, [level, op](QCPAxis* axis) {
		if (level & k_axis_title)
			axis->setLabelColor(op(axis->labelColor()));
		if (level & k_axis_label)
			axis->setTickLabelColor(op(axis->tickLabelColor()));
	});
}


void KcThemedQCP::setTickLength(int level, KeTickSide side, int len)
{
	kPrivate::for_axis(qcp_.get(), level, [this, level, side, len](QCPAxis* axis) {
		if (level & k_axis_tick_major) {
			auto tl = len;
			if (tl == -1)
				tl = std::max(axis->tickLengthIn(), axis->tickLengthOut());

			switch (side) {
			case k_tick_inside:
				axis->setTickLengthIn(tl);
				axis->setTickLengthOut(0);
				break;

			case k_tick_outside:
				axis->setTickLengthIn(0);
				axis->setTickLengthOut(tl);
				break;

			case k_tick_bothside:
				axis->setTickLengthIn(tl);
				axis->setTickLengthOut(tl);
				break;

			case k_tick_none:
			default:
				if(axis->tickLengthIn() > 0)
					axis->setTickLengthIn(tl);
				if (axis->tickLengthOut() > 0)
					axis->setTickLengthOut(tl);
				break;
			}
		}

		if (level & k_axis_tick_minor) {
			auto tl = len;
			if (tl == -1)
				tl = std::max(axis->subTickLengthIn(), axis->subTickLengthOut());

			switch (side) {
			case k_tick_inside:
				axis->setSubTickLengthIn(tl);
				axis->setSubTickLengthOut(0);
				break;

			case k_tick_outside:
				axis->setSubTickLengthIn(0);
				axis->setSubTickLengthOut(tl);
				break;

			case k_tick_bothside:
				axis->setSubTickLengthIn(tl);
				axis->setSubTickLengthOut(tl);
				break;

			case k_tick_none:
			default:
				if (axis->subTickLengthIn() > 0)
					axis->setSubTickLengthIn(tl);
				if (axis->subTickLengthOut() > 0)
					axis->setSubTickLengthOut(tl);
				break;
			}
		}
		});
}


unsigned KcThemedQCP::numPlots() const
{
	return qcp_->axisRect()->plottables().count(); // TODO: multi-panels
}


void KcThemedQCP::applyPalette(unsigned plotIdx, const QColor& major, const QColor& minor)
{
	// TOOD: 越界检测
	auto plot = qcp_->axisRect()->plottables().at(plotIdx);
	applyPalette(plot, major, minor);
}


void KcThemedQCP::applyPalette(QCPAbstractPlottable* plot, const QColor& major_, const QColor& minor)
{
	auto major = major_;
	major.setAlphaF(1.0); // TODO: 为显示line-fill图的临时举措

	if (dynamic_cast<QCPGraph*>(plot)) {
		auto brush = plot->brush();
		auto pen = plot->pen();
		if (brush.style() == Qt::NoBrush) {
			pen.setColor(major);
			brush.setColor(minor);
		}
		else {
			pen.setColor(minor);
			major.setAlphaF(0.25); // TODO: 为显示line-fill图的临时举措
			brush.setColor(major);
		}

		plot->setPen(pen);
		plot->setBrush(brush);

		auto scatter = dynamic_cast<QCPGraph*>(plot)->scatterStyle();
		auto shape = scatter.shape();
		switch (shape) {
		case QCPScatterStyle::ssNone:
			return;

		case QCPScatterStyle::ssDot:
		case QCPScatterStyle::ssCross:
		case QCPScatterStyle::ssPlus:
		case QCPScatterStyle::ssDisc:
		case QCPScatterStyle::ssStar:
			// pen为主色
			if (scatter.isPenDefined()) {
				auto pen = scatter.pen();
				pen.setColor(major);
				scatter.setPen(pen);
			}
			break;

		default: {
			// brush为主色
			auto pen = scatter.pen();
			pen.setColor(minor);
			scatter.setPen(pen);

			auto brush = scatter.brush();
			//brush.setColor(major);
			brush = major;
			scatter.setBrush(brush);
		}
		}

		dynamic_cast<QCPGraph*>(plot)->setScatterStyle(scatter);
	}
	else {
		auto pen = plot->pen();
		pen.setColor(minor);
		plot->setPen(pen);

		auto brush = plot->brush();
		//brush.setColor(major);
		brush = major;
		plot->setBrush(brush);
	}
}


bool KcThemedQCP::legendVisible() const
{
	return qcp_->legend->visible();
}


void KcThemedQCP::setLegendVisible(bool b)
{
	if (legendPlacement() == k_place_outter) {
		if (!b)
			takeLegend_(); // 调整grid
		else
			putLegend_(qcp_->legend, k_place_outter, legendAlign);
	}

	qcp_->legend->setVisible(b);
}


KcThemedQCP::KeLegendPlacement KcThemedQCP::legendPlacement()
{
	if (legendVisible())
		return qcp_->legend->layout()->layout() ? k_place_outter : k_place_inner;
	else
		return qcp_->legend->layout() ? k_place_inner : k_place_outter;
}


void KcThemedQCP::setLegendPlacement(KeLegendPlacement lp)
{
	if (lp == legendPlacement())
		return;

	auto legend = takeLegend_();

	if (legendVisible() || lp == k_place_inner) {
		auto align = legendAlignment();
		putLegend_(legend, lp, align);
	}
}


int KcThemedQCP::legendAlignment()
{
	return legendAlign;
}


namespace kPrivate
{
	static Qt::Alignment toQtAligment(int legendAlignment) 
	{
		if(legendAlignment == KcThemedQCP::k_align_auto)
		    return Qt::AlignJustify | Qt::AlignTop;

		Qt::Alignment qa;

		bool horted(false), verted(false);

		if (legendAlignment & KcThemedQCP::k_align_left) {
			qa |= Qt::AlignLeft;
			horted = true;
		}
		else if (legendAlignment & KcThemedQCP::k_align_right) {
			qa |= Qt::AlignRight;
			horted = true;
		}

		if (legendAlignment & KcThemedQCP::k_align_top) {
			qa |= Qt::AlignTop;
			verted = true;
		}
		else if (legendAlignment & KcThemedQCP::k_align_bottom) {
			qa |= Qt::AlignBottom;
			verted = true;
		}

		if (legendAlignment & KcThemedQCP::k_align_center) {
			if (!horted) qa |= Qt::AlignHCenter;
			if (!verted) qa |= Qt::AlignVCenter;
		}

		return qa;
	}
}


void KcThemedQCP::setLegendAlignment(int align)
{
	if (align == legendAlignment())
		return;

	legendAlign = align;

	if (legendVisible() || legendPlacement() == k_place_inner) 
		putLegend_(takeLegend_(), legendPlacement(), align);
}


KcThemedQCP::KeLegendArrangement KcThemedQCP::legendArrangement()
{
	return qcp_->legend->fillOrder() == QCPLayoutGrid::foColumnsFirst ?
		k_arrange_row : k_arrange_column;
}


void KcThemedQCP::setLegendArrangement(KeLegendArrangement la)
{
	if (la == legendArrangement())
		return;

	qcp_->legend->setFillOrder(la == k_arrange_row ? 
		QCPLayoutGrid::foColumnsFirst : QCPLayoutGrid::foRowsFirst);

	auto layout = qcp_->legend->layout();
	if (layout) {
		auto minSize = qcp_->legend->minimumOuterSizeHint();
		layout->setMinimumSize({ minSize.width(), minSize.height() });
	}
}


std::pair<int, int> KcThemedQCP::legendSpacing()
{
	auto margins = qcp_->legend->minimumMargins();
	return { margins.left(), margins.top() };
}


void KcThemedQCP::setLegendSpacing(int xspacing, int yspacing)
{
	qcp_->legend->setMinimumMargins({ xspacing, yspacing, xspacing, yspacing });
}


QCPLegend* KcThemedQCP::takeLegend_()
{
	auto layout = qcp_->legend->layout();
	if (layout) {
		assert(dynamic_cast<QCPLayoutInset*>(layout));
		layout->take(qcp_->legend);

		QSet<QCPMarginGroup*> groups;
		for (auto mg : layout->marginGroups())
			groups.insert(mg);

		for (auto mg : groups) {
			mg->clear();
			delete mg; // TODO: 是否需要显式delete
		}

		auto layoutp = layout->layout();
		if (layoutp) {
			layoutp->remove(layout);
			layoutp->simplify();
		}
	}

	return qcp_->legend;
}


void KcThemedQCP::putLegend_(QCPLegend* legend, KeLegendPlacement place, int align)
{
	if (place == k_place_inner) {
		auto grid = dynamic_cast<QCPLayoutInset*>(qcp_->axisRect()->insetLayout());
		grid->addElement(legend, kPrivate::toQtAligment(align));
	}
	else {
		auto grid = dynamic_cast<QCPLayoutGrid*>(qcp_->axisRect()->layout());
		
		int idx(0);
		for (; idx < grid->elementCount(); idx++)
			if (dynamic_cast<QCPAxisRect*>(grid->elementAt(idx)))
				break;
		assert(idx < grid->elementCount());

		if (align == k_align_center ||
			align == k_align_auto)
			align = k_align_top;

		bool verted = align & (k_align_bottom | k_align_top);
		bool horzed = align & (k_align_left | k_align_right);
		if (horzed && verted) {
			if (align & k_align_horz_first)
				verted = false; // 如果legend横向排列，则优先将legend防止在top/bottom位置
			else
				horzed = false; // 如果legend纵向排列，则优先将legend防止在left/right位置
		}

		int row, col;
		grid->indexToRowCol(idx, row, col);

		if (verted) {
			if (align & k_align_bottom)
				++row;

			grid->insertRow(row);

			// set legend's row stretch factor very samll so it ends up with minimum height
			grid->setRowStretchFactor(row, 0.001);
		}
		else {
			if (align & k_align_right)
				++col;

			grid->insertColumn(col);
			
			// set legend's column stretch factor very samll so it ends up with minimum width
			grid->setColumnStretchFactor(col, 0.001);
		}

		//legend->setMinimumMargins({ 0, 0, 0, 0 });
		auto minSize = legend->minimumOuterSizeHint();
		auto inset = new QCPLayoutInset;
		inset->setMinimumSize({ minSize.width(), minSize.height() });	
		inset->addElement(legend, kPrivate::toQtAligment(align));
		grid->addElement(row, col, inset);

		auto* group = new QCPMarginGroup(qcp_.get());
		QCP::MarginSides sides = verted ? QCP::msLeft | QCP::msRight : QCP::msTop | QCP::msBottom;
		qcp_->axisRect()->setMarginGroup(sides, group);
		inset->setMarginGroup(sides, group);
	}
}

