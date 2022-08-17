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

	legendAlign = k_align_right | k_align_top;
}


KcThemedQCP::~KcThemedQCP()
{

}


QBrush KcThemedQCP::background() const
{
	return bkgnd_;
}


void KcThemedQCP::setBackground(const QBrush& brush)
{
	bkgnd_ = brush;
	qcp_->setBackground(bkgnd_);
}


QBrush KcThemedQCP::axisBackground() const
{
	auto rect = qcp_->axisRect();
	return rect ? qcp_->axisRect()->backgroundBrush() : QBrush(Qt::NoBrush);
}


void KcThemedQCP::setAxisBackground(const QBrush& brush)
{
	for (auto rect : qcp_->axisRects())
		rect->setBackground(brush);
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
	else if (dynamic_cast<QCPBars*>(plot)) {
		auto pen = plot->pen();
		pen.setColor(minor);
		plot->setPen(pen);

		auto brush = plot->brush();
		brush.setColor(major);
		plot->setBrush(brush);
	}
}


void KcThemedQCP::setVisible(int level, bool b)
{
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

		if ((level & k_axis_label) == k_axis_label) // TODO: 统一
			axis->setTickLabels(b);

		// TODO: axis->setLabel

	});
}


QMargins KcThemedQCP::margins() const
{
	auto layout = qcp_->plotLayout();
	return layout->elementAt(0)->minimumMargins();
}


void KcThemedQCP::setMargins(const QMargins& margins)
{
	auto layout = qcp_->plotLayout();
	for (int i = 0; i < layout->elementCount(); i++) 
		layout->elementAt(i)->setMinimumMargins(margins);
}


KcThemedQCP::KeLegendPlacement KcThemedQCP::legendPlacement()
{
	return dynamic_cast<QCPLayoutInset*>(qcp_->legend->parentLayerable()) ?
		k_place_inner : k_place_outter;
}


void KcThemedQCP::setLegendPlacement(KeLegendPlacement lp)
{
	if (lp == legendPlacement())
		return;

	auto align = legendAlignment();
	putLegend(takeLegend(), lp, align);
}


int KcThemedQCP::legendAlignment()
{
	return legendAlign;
}


namespace kPrivate
{
	static Qt::Alignment toQtAligment(int legendAlignment) 
	{
		Qt::Alignment qa = Qt::AlignJustify;

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

	auto place = legendPlacement();
	putLegend(takeLegend(), place, align);
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


QCPLegend* KcThemedQCP::takeLegend()
{
	auto place = legendPlacement();

	auto layout = dynamic_cast<QCPLayout*>(qcp_->legend->parentLayerable());
	assert(layout);
	layout->take(qcp_->legend);
	layout->simplify();
	
	/*if (place == k_place_outter) {
		auto playout = dynamic_cast<QCPLayout*>(layout->parentLayerable());
		assert(playout);
		playout->remove(layout);
		playout->simplify();
	}*/

	QSet<QCPMarginGroup*> groups;
	for (auto mg : qcp_->legend->marginGroups())
		groups.insert(mg);

	for (auto mg : groups) {
		mg->clear();
		delete mg; // TODO: 是否需要显式delete
	}
	
	return qcp_->legend;
}


void KcThemedQCP::putLegend(QCPLegend* legend, KeLegendPlacement place, int align)
{
	if (place == k_place_inner) {
		auto grid = dynamic_cast<QCPLayoutInset*>(qcp_->axisRect()->insetLayout());
		grid->addElement(legend, kPrivate::toQtAligment(align));
	}
	else {
		auto grid = dynamic_cast<QCPLayoutGrid*>(qcp_->axisRect()->parentLayerable());
		
		int idx(0);
		for (; idx < grid->elementCount(); idx++)
			if (dynamic_cast<QCPAxisRect*>(grid->elementAt(idx)))
				break;
		assert(idx < grid->elementCount());

		if (align == k_align_center ||
			align == k_align_auto)
			align = k_align_top;

		bool verted = align & (k_align_bottom | k_align_top);
		bool horted = align & (k_align_left | k_align_right);
		if (horted && verted) {
			if (legendArrangement() == k_arrange_row)
				horted = false; // 如果legend横向排列，则优先将legend防止在top/bottom位置
			else
				verted = false; // 如果legend纵向排列，则优先将legend防止在left/right位置
		}

		int row, col;
		grid->indexToRowCol(idx, row, col);

		if (verted) {
			if (align & k_align_bottom)
				++row;

			grid->insertRow(row);

			// set legend's row stretch factor very samll so it ends up with minimum height
			grid->setRowStretchFactor(row, 0.0001);
		}
		else {
			if (align & k_align_right)
				++col;

			grid->insertColumn(col);
			grid->setColumnStretchFactor(col, 0.0001);
		}

		grid->addElement(row, col, legend);

		auto* group = new QCPMarginGroup(qcp_.get());
		QCP::MarginSides sides = verted ? QCP::msLeft | QCP::msRight : QCP::msTop | QCP::msBottom;
		qcp_->axisRect()->setMarginGroup(sides, group);
		legend->setMarginGroup(sides, group);
	}
}

