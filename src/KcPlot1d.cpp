#include "KcPlot1d.h"
#include <assert.h>
#include "KcDataSnapshot.h"
#include "KvData1d.h"
#include <QBrush>
#include "qcustomplot/qcustomplot.h"


namespace kPrivate
{
	static QString typeToStr(KcPlot1d::KeType type)
	{
		switch (type)
		{
		case KcPlot1d::KeType::k_scatter:
			return QObject::tr(u8"scatter_plot");

		case KcPlot1d::KeType::k_bars:
			return QObject::tr(u8"bars_plot");

		case KcPlot1d::KeType::k_line:
		default:
			return QObject::tr(u8"line_plot");
		}
	}
}


KcPlot1d::KcPlot1d(KvDataProvider* is, KeType type)
	: KvCustomPlot(is, kPrivate::typeToStr(type))
	, type_(type)
{
	if (type == KeType::k_scatter) {
		auto graph = customPlot_->addGraph();
		graph->setLineStyle(QCPGraph::lsNone);
		graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross));
		graph->setAntialiasedScatters(true);
	}
	else if (type == KeType::k_bars) {
		auto bars = new QCPBars(customPlot_->xAxis, customPlot_->yAxis);
		bars->setWidthType(QCPBars::wtPlotCoords);
		barWidthRatio_ = 0.5f;
		updateBarWidth_();
	}
	else {
		assert(type == KeType::k_line);
		auto graph = customPlot_->addGraph();
		graph->setAdaptiveSampling(true);
	}
}


namespace kPrivate
{
	enum KePlot1dProperty
	{
		k_plot1d_prop_id = 200, // 此前的id预留给KvCustomPlot

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
	bool doPlot(QCPAbstractPlottable* plot, std::shared_ptr<KvData1d> data1d, bool streaming)
	{
		auto graph = dynamic_cast<PLOT_TYPE*>(plot);
		if (graph == nullptr)
			return false;


		if (streaming) {
			auto dataRange = data1d->range(0);
			auto plotRange = plot->keyAxis()->range();

			auto duration = dataRange.length();
			auto data = graph->data();
			data->removeBefore(duration + plotRange.lower); // 前推已有数据

			QVector<double> keys, values;
			keys.reserve(data1d->count());
			values.reserve(data1d->count());
			if (duration < plotRange.upper - +plotRange.lower) { // 新数据比已有数据少, 须保留部分旧数据
				for (unsigned i = 0; i < data->size(); i++) {
					auto iter = data->at(i);
					keys.push_back(iter->key - duration); // 修正残留数据的key值
					values.push_back(iter->value);
				}
			}

			// 新数据key值的偏移 
			auto keyOffset = plotRange.upper - dataRange.high();

			for (kIndex idx = 0; idx < data1d->count(); idx++) {
				auto val = data1d->value(idx);
				auto key = val.x + keyOffset;
				if (key < plotRange.lower)
					continue;

				keys.push_back(key);
				values.push_back(val.y);
			}

			graph->setData(keys, values);
		}
		else {
			graph->data()->clear();
			for (kIndex idx = 0; idx < data1d->count(); idx++) {
				auto val = data1d->value(idx);
				graph->addData(val.x, val.y);
			}
		}

		return true;
	}

}


KvPropertiedObject::kPropertySet KcPlot1d::propertySet() const
{
	using namespace kPrivate;

	kPropertySet ps = KvCustomPlot::propertySet();

	KpProperty prop;
	KpProperty subProp;

	if(type_ == KeType::k_line)
		ps.push_back(lineProperty_(false));
	else if(type_ == KeType::k_bars)
		ps.push_back(barProperty_());


	if (type_ != KeType::k_bars)
		ps.push_back(scatterProperty_(type_ == KeType::k_line));

	return ps;
}


KvPropertiedObject::KpProperty KcPlot1d::scatterProperty_(bool hasNone) const
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
	for (unsigned i = hasNone ? 0 : 1; i < sizeof(shapes) / sizeof(std::pair<QString, int>); i++) {
		KvPropertiedObject::KpProperty sub;
		sub.name = shapes[i].first;
		sub.val = shapes[i].second;
		subProp.children.push_back(sub);
	}
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

KvPropertiedObject::KpProperty KcPlot1d::lineProperty_(bool hasNone) const
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

	for (unsigned i = hasNone ? 0 : 1; i < sizeof(styles) / sizeof(std::pair<QString, int>); i++) {
		KvPropertiedObject::KpProperty sub;
		sub.name = styles[i].first;
		sub.val = styles[i].second;
		subProp.children.push_back(sub);
	}
	prop.children.push_back(subProp);

	subProp.id = kPrivate::k_line_pen;
	subProp.name = QStringLiteral("Pen");
	subProp.val = customPlot_->plottable()->pen();
	subProp.attr.penFlags = KvPropertiedObject::k_pen_all;
	prop.children.push_back(subProp);

	return prop;
}


KvPropertiedObject::KpProperty KcPlot1d::barProperty_() const
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


void KcPlot1d::onPropertyChanged(int id, const QVariant& newVal)
{
	using namespace kPrivate;

	assert(id >= 0);

	if (id <= k_plot1d_prop_id) {
		KvCustomPlot::onPropertyChanged(id, newVal);
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
		}
	}

	if (customPlot_->isVisible()) 	
		customPlot_->replot();
}


bool KcPlot1d::render(std::shared_ptr<KvData> data)
{
	auto plot = customPlot_->plottable();

	assert(data->dim() == 1);
	auto data1d = std::dynamic_pointer_cast<KvData1d>(data);
	assert(data1d->channels() == 1); // TODO: 暂且假定单通道数据

	auto prov = dynamic_cast<KvDataProvider*>(parent());

	if (type_ == KeType::k_bars) {
		kPrivate::doPlot<QCPBars>(plot, data1d, prov->isStream());
		updateBarWidth_();
	}
	else {
		kPrivate::doPlot<QCPGraph>(plot, data1d, prov->isStream());
	}

	show(true);

	customPlot_->replot(prov->isStream() 
		? QCustomPlot::rpQueuedRefresh 
		: QCustomPlot::rpRefreshHint);

	return true;
}


void KcPlot1d::reset()
{
	auto plot = customPlot_->plottable();
	if (type_ == KeType::k_bars)
		kPrivate::clearData<QCPBars>(plot);
	else 
		kPrivate::clearData<QCPGraph>(plot);
}


void KcPlot1d::updateBarWidth_()
{
	auto prov = dynamic_cast<KvDataProvider*>(parent());
	auto bars = dynamic_cast<QCPBars*>(customPlot_->plottable());
	auto dx = prov->step(0);
	if (dx == KvData::k_unknown_step || dx == KvData::k_nonuniform_step) {
		if (bars->dataCount() > 0)
			dx = prov->range(0).length() / bars->dataCount();
		else
			dx = 1; // 随意默认值
	}

	bars->setWidth(dx * barWidthRatio_);
}