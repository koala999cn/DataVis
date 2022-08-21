﻿#include "KcRdPlot1d.h"
#include <assert.h>
#include "prov/KcPvData.h"
#include "KvData.h"
#include "KtSampling.h"
#include "KvContinued.h"
#include "theme/KcThemedQCP.h"
#include <QBrush>
#include "qcustomplot/qcustomplot.h"


namespace kPrivate
{
	static QString typeToStr(KcRdPlot1d::KeType type)
	{
		static const char* typeText[] = {
			"scatter",
			"line",
			"line-scatter",
			"line-fill",
			"stacked-bars",
			"grouped-bars"
		};

		return typeText[type];
	}
}


KcRdPlot1d::KcRdPlot1d(KvDataProvider* is)
	: KvRdCustomPlot(is, "plot1d")
	, delayedChangeType_(false)
{
	barsGroup_ = new QCPBarsGroup(customPlot_);

	// 根据is自动选择最优绘图类型
	if (is->isScattered())
		type_ = k_scatter;
	else if (is->isContinued())
		type_ = k_line;
	else 
		type_ = is->size() > 16 ? k_line : k_bars_grouped;

	changePlotType_();

	if (is->isContinued()) {

		connect(customPlot_->xAxis, qOverload<const QCPRange&>(&QCPAxis::rangeChanged),
			[this](const QCPRange& newRange) {
				if (contData_)
					doRender_(contData_);
			});

		connect(customPlot_->yAxis, qOverload<const QCPRange&>(&QCPAxis::rangeChanged),
			[this](const QCPRange& newRange) {
				if (contData_)
					doRender_(contData_);
			});
	}
}


namespace kPrivate
{
	enum KePlot1dProperty
	{
		k_plot1d_prop_id = 200, // 此前的id预留给KvRdCustomPlot

		k_type,

		k_line_pen,
		k_line_style,

		k_scatter_shape,
		k_scatter_size,
		k_scatter_skip,
		k_scatter_pen,
		k_scatter_brush,

		k_bar_pen,
		k_bar_width,
		k_bar_fill_brush,
		k_bar_fill_color,

		k_legend_visible,
		k_legend_place,
		k_legend_align,
		k_legend_arrange
	};


	template<typename PLOT_TYPE>
	bool clearData(QCPAbstractPlottable* plot)
	{
		auto graph = dynamic_cast<PLOT_TYPE*>(plot);
		if (graph) {
			graph->data()->clear();
			return true;
		}

		return false;
	}


	template<typename PLOT_TYPE>
	bool doPlot(QCPAbstractPlottable* plot, std::shared_ptr<KvData> data1d, kIndex ch, bool streaming)
	{
		assert(data1d->dim() == 1);

		auto graph = dynamic_cast<PLOT_TYPE*>(plot);
		if (graph == nullptr)
			return false;

		if (streaming) {
			assert(data1d->isDiscreted()); // TODO: 处理连续数据
			auto dis = std::dynamic_pointer_cast<KvDiscreted>(data1d);
			auto dataRange = data1d->range(0);
			auto plotRange = plot->keyAxis()->range();

			auto duration = dataRange.length();
			auto gdata = graph->data();
			gdata->removeBefore(duration + plotRange.lower); // 前推已有数据

			QVector<double> keys, values;
			keys.reserve(data1d->size());
			values.reserve(data1d->size());

			// 修正残留数据的key值
			for (unsigned i = 0; i < gdata->size(); i++) {
				auto iter = gdata->at(i);
				keys.push_back(iter->key - duration); 
				values.push_back(iter->value);
			}

			// 新数据key值的偏移 
			auto keyOffset = plotRange.upper - dataRange.high();

			for (kIndex idx = 0; idx < data1d->size(); idx++) {
				auto val = dis->pointAt(idx, ch);
				auto key = val[0] + keyOffset;
				if (key < plotRange.lower)
					continue;

				keys.push_back(key);
				values.push_back(val[1]);
			}

			graph->setData(keys, values);
		}
		else {
			graph->data()->clear();
			if (data1d->isDiscreted()) {
				auto dis = std::dynamic_pointer_cast<KvDiscreted>(data1d);
				for (kIndex idx = 0; idx < dis->size(); idx++) {
					auto val = dis->pointAt(idx, ch);
					graph->addData(val[0], val[1]);
				}
			}
			else {
				auto cond = std::dynamic_pointer_cast<KvContinued>(data1d);
				auto r = plot->keyAxis()->range();
				KtSampling<kReal> samp;
				samp.resetn(1000, r.lower, r.upper, 0.5); // TODO:
				for (kIndex i = 0; i < samp.size(); i++) {
					kReal x = samp.indexToX(i);
					auto y = cond->value(x, ch);
					graph->addData(x, y);
				}
			}
		}

		return true;
	}

}


KvPropertiedObject::kPropertySet KcRdPlot1d::propertySet() const
{
	using namespace kPrivate;

	kPropertySet ps = KvRdCustomPlot::propertySet();

	KpProperty prop;
	KpProperty subProp;

	prop.id = k_type;
	prop.name = "Type";
	QStringList sl;
	for (int i = 0; i < k_type_count; i++)
		sl << typeToStr(KeType(i));
	prop.makeEnum(sl);
	prop.val = type_;
	ps.push_back(prop);

	prop.id = k_legend_visible;
	prop.name = "Legend";
	prop.val = themedPlot_->visible(KvThemedPlot::k_legend);
	prop.flag = 0;
	prop.enumList.clear();
	
	subProp.id = k_legend_place;
	subProp.name = "Placement";
	std::pair<QString, int> place[] = {
		{ "inner", KvThemedPlot::k_place_inner },
		{ "outter", KvThemedPlot::k_place_outter }
	};
	subProp.makeEnum(place);
	subProp.val = themedPlot_->legendPlacement();
	prop.children.push_back(subProp);

	subProp.id = k_legend_align;
	subProp.name = "Aligment";
	std::pair<QString, int> align[] = {
		//{ "auto", KvThemedPlot::k_align_auto }, // TODO: auto没效果
		{ "top", KvThemedPlot::k_align_top },
		{ "right", KvThemedPlot::k_align_right },
		{ "bottom", KvThemedPlot::k_align_bottom },
		{ "left", KvThemedPlot::k_align_left },
		{ "center", KvThemedPlot::k_align_center },
		{ "top-left", KvThemedPlot::k_align_top | KvThemedPlot::k_align_left | KvThemedPlot::k_align_vert_first },
		{ "top-right", KvThemedPlot::k_align_top | KvThemedPlot::k_align_right | KvThemedPlot::k_align_vert_first },
		{ "right-top", KvThemedPlot::k_align_top | KvThemedPlot::k_align_right | KvThemedPlot::k_align_horz_first },
		{ "right-bottom", KvThemedPlot::k_align_bottom | KvThemedPlot::k_align_right | KvThemedPlot::k_align_horz_first },
		{ "bottom-left", KvThemedPlot::k_align_bottom | KvThemedPlot::k_align_left | KvThemedPlot::k_align_vert_first },
		{ "bottom-right", KvThemedPlot::k_align_bottom | KvThemedPlot::k_align_right | KvThemedPlot::k_align_vert_first },
		{ "left-top", KvThemedPlot::k_align_top | KvThemedPlot::k_align_left | KvThemedPlot::k_align_horz_first },
		{ "left-bottom", KvThemedPlot::k_align_bottom | KvThemedPlot::k_align_left | KvThemedPlot::k_align_horz_first },
	};
	subProp.makeEnum(align);
	subProp.val = themedPlot_->legendAlignment();
	prop.children.push_back(subProp);

	subProp.id = k_legend_arrange;
	subProp.name = "Arrange";
	std::pair<QString, int> arrange[] = {
		{ "row", KvThemedPlot::k_arrange_row },
		{ "column", KvThemedPlot::k_arrange_column }
	};
	subProp.makeEnum(arrange);
	subProp.val = themedPlot_->legendArrangement();
	prop.children.push_back(subProp);

	ps.push_back(prop);

	return ps;
}


KvPropertiedObject::KpProperty KcRdPlot1d::scatterProperty_(bool hasNone) const
{
	KpProperty prop;
	KpProperty subProp;

	prop.id = KvPropertiedObject::kInvalidId;
	prop.name = QStringLiteral("Scatter");
	prop.flag = KvPropertiedObject::k_collapsed;

	auto style = customPlot_->graph()->scatterStyle();

	static const std::pair<QString, int> shapes[] = {
		{ "None", QCPScatterStyle::ssNone },
		{ "Dot", QCPScatterStyle::ssDot },
		{ "Corss", QCPScatterStyle::ssCross },
		{ "Plus", QCPScatterStyle::ssPlus },
		{ "Circle", QCPScatterStyle::ssCircle },
		{ "Disc", QCPScatterStyle::ssDisc },
		{ "Square", QCPScatterStyle::ssSquare },
		{ "Diamond", QCPScatterStyle::ssDiamond },
		{ "Star", QCPScatterStyle::ssStar },
		{ "Triangle", QCPScatterStyle::ssTriangle },
		{ "TriangleInverted", QCPScatterStyle::ssTriangleInverted },
		{ "CrossSquare", QCPScatterStyle::ssCrossSquare },
		{ "PlusSquare", QCPScatterStyle::ssPlusSquare },
		{ "CrossCircle", QCPScatterStyle::ssCrossCircle },
		{ "PlusCircle", QCPScatterStyle::ssPlusCircle },
		{ "Peace", QCPScatterStyle::ssPeace }
	};

	subProp.id = kPrivate::k_scatter_shape;
	subProp.name = u8"Shape";
	subProp.desc.clear();
	subProp.val = QVariant::fromValue<int>(style.shape()); // int类型代表enum类型
	subProp.makeEnum(shapes);
	prop.children.push_back(subProp);
	
	subProp.id = kPrivate::k_scatter_size;
	subProp.name = u8"Size";
	subProp.val = style.size();
	subProp.minVal = 0.0;
	subProp.maxVal = 10.0;
	subProp.step = 1.0;
	subProp.children.clear();
	prop.children.push_back(subProp);

	if (type_ == KeType::k_line) {
		subProp.id = kPrivate::k_scatter_skip;
		subProp.name = u8"Skip";
		subProp.val = customPlot_->graph()->scatterSkip();
		subProp.minVal = 0;
		subProp.step = 1;
		prop.children.push_back(subProp);
	}

	subProp.id = kPrivate::k_scatter_pen;
	subProp.name = u8"Pen";
	if (!style.isPenDefined()) {
		style.setPen(customPlot_->graph()->pen());
		customPlot_->graph()->setScatterStyle(style);
	}
	subProp.val = style.pen();
	subProp.attr.penFlags = KvPropertiedObject::k_pen_color | KvPropertiedObject::k_pen_width;
	prop.children.push_back(subProp);

    return prop;
}

KvPropertiedObject::KpProperty KcRdPlot1d::lineProperty_(bool hasNone) const
{
	KpProperty prop;
	KpProperty subProp;

	prop.id = KvPropertiedObject::kInvalidId;
	prop.name = QStringLiteral("Line");
	prop.flag = KvPropertiedObject::k_collapsed;

	subProp.id = kPrivate::k_line_style;
	subProp.name = "LineStyle";
	subProp.desc = tr("Line style");
	subProp.val = QVariant::fromValue<int>(customPlot_->graph()->lineStyle());

	static const std::pair<QString, int> styles[] = {
		{ "None", QCPGraph::lsNone },
		{ "Line", QCPGraph::lsLine },
		{ "StepLeft", QCPGraph::lsStepLeft },
		{ "StepRight", QCPGraph::lsStepRight },
		{ "StepCenter", QCPGraph::lsStepCenter },
		{ "Impulse", QCPGraph::lsImpulse }
	};
	subProp.enumList.clear();
	for (unsigned i = hasNone ? 0 : 1; i < sizeof(styles) / sizeof(std::pair<QString, int>); i++) 
		subProp.enumList.emplace_back(styles[i].first, styles[i].second);
	prop.children.push_back(subProp);

	subProp.id = kPrivate::k_line_pen;
	subProp.name = QStringLiteral("Pen");
	subProp.val = customPlot_->plottable()->pen();
	subProp.attr.penFlags = KvPropertiedObject::k_pen_all;
	prop.children.push_back(subProp);

	return prop;
}


KvPropertiedObject::KpProperty KcRdPlot1d::barProperty_() const
{
	KpProperty prop;
	KpProperty subProp;

	auto bars = dynamic_cast<QCPBars*>(customPlot_->plottable());
	assert(bars);

	prop.id = KvPropertiedObject::kInvalidId;
	prop.name = QStringLiteral("Bar");
	prop.flag = KvPropertiedObject::k_collapsed;

	subProp.id = kPrivate::k_bar_width;
	subProp.name = "Width";
	subProp.val = barWidthRatio_;
	subProp.minVal = 0;
	subProp.maxVal = 1;
	subProp.step = 0.1;
	prop.children.push_back(subProp);


	subProp.id = kPrivate::k_bar_pen;
	subProp.name = QStringLiteral("Pen");
	subProp.val = customPlot_->plottable()->pen();
	subProp.attr.penFlags = KvPropertiedObject::k_pen_color |
		KvPropertiedObject::k_pen_none | KvPropertiedObject::k_pen_width | KvPropertiedObject::k_show_alpha_channel;
	prop.children.push_back(subProp);

	subProp.id = kPrivate::k_bar_fill_brush;
	subProp.name = QStringLiteral("Brush");
	subProp.val = QVariant::fromValue(customPlot_->plottable()->brush());
	subProp.attr.showAllBrushStyle = false;
	prop.children.push_back(subProp);

	
	subProp.id = kPrivate::k_bar_fill_color;
	subProp.name = QStringLiteral("Color");
	subProp.val = QVariant::fromValue(customPlot_->plottable()->brush().color());
	subProp.attr.colorFlags = KvPropertiedObject::k_show_alpha_channel;
	prop.children.push_back(subProp);


	return prop;
}


void KcRdPlot1d::setPropertyImpl_(int id, const QVariant& newVal)
{
	using namespace kPrivate;

	assert(id >= 0);

	if (id <= k_plot1d_prop_id) {
		KvRdCustomPlot::setPropertyImpl_(id, newVal);
	}
	else {

		switch (id) {
		case k_line_pen:
		case k_bar_pen:
			customPlot_->plottable()->setPen(newVal.value<QPen>());
			break;

		case k_scatter_shape: {
			auto style = customPlot_->graph()->scatterStyle();
			style.setShape(QCPScatterStyle::ScatterShape(newVal.toInt()));
			customPlot_->graph()->setScatterStyle(style);
			break;
		    }

		case k_scatter_size: {
			auto style = customPlot_->graph()->scatterStyle();
			style.setSize(newVal.toDouble());
			customPlot_->graph()->setScatterStyle(style);
			break;
		    }

		case k_scatter_pen: {
			auto style = customPlot_->graph()->scatterStyle();
			style.setPen(newVal.value<QPen>());
			customPlot_->graph()->setScatterStyle(style);
			break;
		    }

		case k_scatter_skip:
			customPlot_->graph()->setScatterSkip(newVal.toInt());
			break;

		case k_line_style:
			customPlot_->graph()->setLineStyle(QCPGraph::LineStyle(newVal.toInt()));
			break;

		case k_bar_fill_brush: {
			auto brush = customPlot_->plottable()->brush();
			brush.setStyle(Qt::BrushStyle(newVal.value<qint32>()));
			customPlot_->plottable()->setBrush(brush);
			break;
		    }

		case k_bar_fill_color: {
			auto brush = customPlot_->plottable()->brush();
			brush.setColor(newVal.value<QColor>());
			customPlot_->plottable()->setBrush(brush);
			break;
		    }
			
		case k_bar_width: {
			barWidthRatio_ = newVal.toFloat();
			updateBarWidth_();
			break;
		    }

		case k_type:
			type_ = KeType(newVal.toInt());
			delayedChangeType_ = ((KvDataProvider*)rootParent())->isStream();
			if (!delayedChangeType_) 
				changePlotType_();
			break;

		case k_legend_visible:
			themedPlot_->applyVisible(KvThemedPlot::k_legend, newVal.toBool());
			break;

		case k_legend_place:
			themedPlot_->setLegendPlacement(KvThemedPlot::KeLegendPlacement(newVal.toInt()));
			break;

		case k_legend_align:
			themedPlot_->setLegendAlignment(newVal.toInt());
			break;

		case k_legend_arrange:
			themedPlot_->setLegendArrangement(KvThemedPlot::KeLegendArrangement(newVal.toInt()));
			break;

		default:
			assert(false);
			break;
		}
	}

	if (customPlot_->isVisible())
		customPlot_->replot();
}


bool KcRdPlot1d::doRender_(std::shared_ptr<KvData> data)
{
	assert(data->dim() == 1);
	assert(data->channels() == customPlot_->plottableCount());
	auto streaming = dynamic_cast<KvDataProvider*>(parent())->isStream();

	for (kIndex ch = 0; ch < data->channels(); ch++) {
		auto plot = customPlot_->plottable(ch);
		
		if (dynamic_cast<QCPBars*>(plot)) {
			kPrivate::doPlot<QCPBars>(plot, data, ch, streaming);
			updateBarWidth_();
		}
		else if (dynamic_cast<QCPGraph*>(plot)) {
			kPrivate::doPlot<QCPGraph>(plot, data, ch, streaming);
		}
	}

	customPlot_->replot(streaming
		? QCustomPlot::rpQueuedRefresh : QCustomPlot::rpRefreshHint);

	return true;
}


void KcRdPlot1d::reset()
{
	for (int i = 0; i < customPlot_->plottableCount(); i++) {
		auto plot = customPlot_->plottable(i);
		if (dynamic_cast<QCPBars*>(plot))
			kPrivate::clearData<QCPBars>(plot);
		else if(dynamic_cast<QCPGraph*>(plot))
			kPrivate::clearData<QCPGraph>(plot);
		else
			assert(false);
	}
}


void KcRdPlot1d::updateBarWidth_()
{
	int numBars(0);
	for (int i = 0; i < customPlot_->plottableCount(); i++) 
		if (dynamic_cast<QCPBars*>(customPlot_->plottable(i)))
			++numBars;

	if (numBars == 0)
		return;

	auto prov = dynamic_cast<KvDataProvider*>(parent());

	for (int i = 0; i < customPlot_->plottableCount(); i++) {

		auto bars = dynamic_cast<QCPBars*>(customPlot_->plottable(i));
		if (!bars) continue;

		auto dx = prov->step(0);
		if (dx == 0) {
			if (bars->dataCount() > 0)
				dx = prov->range(0).length() / bars->dataCount();
			else
				dx = 1; // 随意默认值
		}

		auto width = bars->barsGroup() ? dx * barWidthRatio_ / numBars : dx * barWidthRatio_;
		bars->setWidth(width);
	}
}


void KcRdPlot1d::preRender_()
{
	if (delayedChangeType_) {
		delayedChangeType_ = false;
		changePlotType_();
	}
}


void KcRdPlot1d::changePlotType_()
{
	auto objp = static_cast<KvDataProvider*>(parent());

	std::vector< QCPAbstractPlottable*> oldPlots(customPlot_->plottableCount());
	for (int i = 0; i < customPlot_->plottableCount(); i++)
		oldPlots[i] = customPlot_->plottable(i);

	std::vector< QCPAbstractPlottable*> newPlots(objp->channels());
	
	for (kIndex ch = 0; ch < objp->channels(); ch++)
		newPlots[ch] = clonePlottable_(ch < oldPlots.size() ? oldPlots[ch] : nullptr, type_);

	for (auto i : oldPlots)
		customPlot_->removePlottable(i);

	updateBarWidth_();
}


QCPAbstractPlottable* KcRdPlot1d::clonePlottable_(QCPAbstractPlottable* oldPlot, KeType type)
{
	auto newPlot = create_(type);
	if (oldPlot) {
		cloneData_(oldPlot, newPlot);
		cloneTheme_(oldPlot, newPlot);
	}
	return newPlot;
}


QCPAbstractPlottable* KcRdPlot1d::create_(KeType type)
{
	QCPAbstractPlottable* newPlot;

	switch (type_)
	{
	case KcRdPlot1d::k_scatter:
	case KcRdPlot1d::k_line:
	case KcRdPlot1d::k_line_scatter:
	case KcRdPlot1d::k_line_fill:

	{
		auto graph = customPlot_->addGraph();
		graph->setAntialiasedScatters(true);
		graph->setAdaptiveSampling(true);

		if (type_ == k_scatter) {
			graph->setLineStyle(QCPGraph::lsNone);
			graph->setScatterStyle(QCPScatterStyle::ssCircle);
		}
		else if (type_ == k_line_fill) {
			auto brush = graph->brush();
			brush.setStyle(Qt::SolidPattern);
			graph->setBrush(brush);
		}
		else if (type_ == k_line_scatter) {
			graph->setScatterStyle(QCPScatterStyle::ssCircle);
		}

		newPlot = graph;
	}

	break;


	case KcRdPlot1d::k_bars_grouped:
	case KcRdPlot1d::k_bars_stacked:

	{
		auto bars = new QCPBars(customPlot_->xAxis, customPlot_->yAxis);
		if (type_ == k_bars_grouped)
			bars->setBarsGroup(barsGroup_);
		bars->setWidthType(QCPBars::wtPlotCoords);
		barWidthRatio_ = 0.5f;
		updateBarWidth_();
		newPlot = bars;
	}
	break;

	default:
		assert(false);
		break;
	}

	return newPlot;
}


void KcRdPlot1d::cloneData_(QCPAbstractPlottable* from, QCPAbstractPlottable* to)
{
	if (dynamic_cast<QCPGraph*>(to)) {
		auto g = dynamic_cast<QCPGraph*>(to);
		g->data()->clear();
		auto fromData = from->interface1D();
		QVector<QCPGraphData> toData;
		toData.reserve(fromData->dataCount());
		for (int i = 0; i < fromData->dataCount(); i++)
			toData.append({ fromData->dataMainKey(i), fromData->dataMainValue(i) });
		g->data()->add(std::move(toData), true);
	}
	else if (dynamic_cast<QCPBars*>(to)) {
		auto g = dynamic_cast<QCPBars*>(to);
		g->data()->clear();
		auto fromData = from->interface1D();
		QVector<QCPBarsData> toData;
		toData.reserve(fromData->dataCount());
		for (int i = 0; i < fromData->dataCount(); i++)
			toData.append({ fromData->dataMainKey(i), fromData->dataMainValue(i) });
		g->data()->add(std::move(toData), true);
	}
}


namespace kPrivate
{
	void swap_color(QPen& pen, QBrush& brush)
	{
		auto pc = pen.color();
		pen.setColor(brush.color());
		brush.setColor(pc);
	}
}

void KcRdPlot1d::cloneTheme_(QCPAbstractPlottable* from, QCPAbstractPlottable* to)
{
	QColor major, minor;
	if (dynamic_cast<QCPGraph*>(from)) {
		auto g = dynamic_cast<QCPGraph*>(from);
		major = g->pen().color();
		minor = g->brush().color();

		if (g->brush().style() != Qt::NoBrush)
			qSwap(major, minor);
	}
	else if (dynamic_cast<QCPBars*>(from)) {
		auto g = dynamic_cast<QCPBars*>(from);
		major = g->brush().color();
		minor = g->pen().color();
	}

	KcThemedQCP::applyPalette(to, major, minor);
}

