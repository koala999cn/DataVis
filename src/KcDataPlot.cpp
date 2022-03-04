#include "KcDataPlot.h"
#include <assert.h>
#include "KcDataSnapshot.h"
#include "KvData1d.h"
#include "QtAppEventHub.h"
#include "base/KuPathUtil.h"
#include <QFileDialog>
#include <QBrush>

#define QCUSTOMPLOT_USE_OPENGL
#define QCUSTOMPLOT_USE_LIBRARY
#include "qcustomplot/qcustomplot.h"


namespace kPrivate
{
	static QString typeToStr(KcDataPlot::KePlot1dType type)
	{
		switch (type)
		{
		case KcDataPlot::KePlot1dType::k_scatter:
			return QObject::tr(u8"scatter_plot");

		case KcDataPlot::KePlot1dType::k_line:
			return QObject::tr(u8"line_plot");

		case KcDataPlot::KePlot1dType::k_bars:
			return QObject::tr(u8"bars_plot");
		}

		assert(false);
		return QObject::tr(u8"unknown");
	}
}


KcDataPlot::KcDataPlot(KvDataProvider* is, KePlot1dType type)
	: KvDataRender(kPrivate::typeToStr(type), is)
	, type_(type)
{
	customPlot_ = new QCustomPlot(nullptr);
	customPlot_->setOpenGl(false); // TODO: QCustomPlot在Opengl模式下有bug，待修复
	rescaleAxes(); // 设置缺省的坐标轴范围
	autoScale_ = !is->isStream();

	// 根据style设置背景色???TODO: 是否需要？前景色呢？
	back_ = QColor(255, 255, 255); // dockFrame->style()->standardPalette().color(QPalette::Base);
	customPlot_->setBackground(QBrush(back_));

	// 设置上下文菜单
	customPlot_->setContextMenuPolicy(Qt::CustomContextMenu);
	customPlot_->connect(customPlot_, &QWidget::customContextMenuRequested, [this](const QPoint& pos) {
		QAction exportAction(QWidget::tr("export..."), customPlot_);
		customPlot_->connect(&exportAction, &QAction::triggered, [this] { exportAs(); });
		QMenu menu(customPlot_);
		menu.addAction(&exportAction);
		menu.exec(customPlot_->mapToGlobal(pos));
	    });

	if (type == KePlot1dType::k_scatter) {
		auto graph = customPlot_->addGraph();
		graph->setLineStyle(QCPGraph::lsNone);
		graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross));
		graph->setAntialiasedScatters(true);
	}
	else if(type == KePlot1dType::k_line) {
		auto graph = customPlot_->addGraph();
	    graph->setAdaptiveSampling(true);
	}
	else if (type == KePlot1dType::k_bars) {
		auto bars = new QCPBars(customPlot_->xAxis, customPlot_->yAxis);
		bars->setWidthType(QCPBars::wtPlotCoords);
		bars->setBaseValue(0);
	}
	else {
		assert(false);
	}
}


KcDataPlot::~KcDataPlot()
{
	customPlot_->setParent(nullptr); // TODO: 这很重要，QCustomPlot析构时貌似没有执行该操作
	delete customPlot_;
}


namespace kPrivate
{
	enum KePlotPropertyId 
	{
		k_auto_scale,
		k_axis_bottom,
		k_axis_bottom_visible = k_axis_bottom,
		k_axis_bottom_range,
		k_axis_bottom_tick_length,
		k_axis_bottom_subtick_length,
		k_axis_left,
		k_axis_left_visible = k_axis_left,
		k_axis_left_range,
		k_axis_left_tick_length,
		k_axis_left_subtick_length,
		k_axis_top,
		k_axis_top_visible = k_axis_top,
		k_axis_top_range,
		k_axis_top_tick_length,
		k_axis_top_subtick_length,
		k_axis_right,
		k_axis_right_visible = k_axis_right,
		k_axis_right_range,
		k_axis_right_tick_length,
		k_axis_right_subtick_length,

		k_background,

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

		k_axis_visible = k_axis_bottom_visible - k_axis_bottom,
		k_axis_range = k_axis_bottom_range - k_axis_bottom,
		k_axis_tick_length = k_axis_bottom_tick_length - k_axis_bottom,
		k_axis_subtick_length = k_axis_bottom_subtick_length - k_axis_bottom,
		k_axis_max = k_axis_subtick_length
	};


	// @idBase: 用来计算真实id的基数
	static KvPropertiedObject::kPropertySet getAxisProperties(QCPAxis* axis, int idBase)
	{
		KvPropertiedObject::kPropertySet psAxis;
		KvPropertiedObject::KpProperty prop;

		prop.id = idBase + k_axis_visible;
		prop.name = QStringLiteral("visible");
		prop.flag = 0;
		prop.val = axis->visible();
		psAxis.push_back(prop);

		prop.id = idBase + k_axis_range;
		prop.name = QStringLiteral("range");
		prop.flag = KvPropertiedObject::k_restrict;
		prop.val = QPointF(axis->range().lower, axis->range().upper);
		KvPropertiedObject::KpProperty subProp;
		subProp.name = QStringLiteral("min");
		prop.children.push_back(subProp);
		subProp.name = QStringLiteral("max");
		prop.children.push_back(subProp);
		psAxis.push_back(prop);

		prop.id = idBase + k_axis_tick_length;
		prop.name = QStringLiteral("tick-length");
		prop.disp = QStringLiteral("tick length");
		prop.flag = 0;
		prop.val = QPoint(axis->tickLengthIn(), axis->tickLengthOut());
		psAxis.push_back(prop);

		prop.id = idBase + k_axis_subtick_length;
		prop.name = QStringLiteral("subtick-length");
		prop.disp = QStringLiteral("subtick length");
		prop.val = QPoint(axis->subTickLengthIn(), axis->subTickLengthOut());
		psAxis.push_back(prop);

		return psAxis;
	}

	static void onAxisPropertyChanged(QCPAxis* axis, int idDiff, const QVariant& newVal)
	{
		switch(idDiff) {
		case k_axis_visible:
			axis->setVisible(newVal.toBool());
			break;

		case k_axis_range:
			axis->setRange(newVal.toPointF().x(), newVal.toPointF().y());
			break;

		case k_axis_tick_length:
			axis->setTickLength(newVal.toPoint().x(), newVal.toPoint().y());
			break;

		case k_axis_subtick_length:
			axis->setSubTickLength(newVal.toPoint().x(), newVal.toPoint().y());
			break;

		default:
			assert(false);
			break;
		}
	}

	/*
	template<typename PLOT_TYPE>
	bool clearPlotData(QCPAbstractPlottable* plot)
	{
		auto graph = dynamic_cast<PLOT_TYPE*>(plot);
		if (graph) {
			graph->data()->clear();
			return true;
		}

		return false;
	}*/

	template<typename PLOT_TYPE>
	bool plotData1d(QCPAbstractPlottable* plot, std::shared_ptr<KvData1d> data1d, bool streaming)
	{
		auto graph = dynamic_cast<PLOT_TYPE*>(plot);
		if (graph == nullptr)
			return false;


		if (streaming) {
			auto dataRange = data1d->xrange();
			auto plotRange = plot->keyAxis()->range();

			auto duration = dataRange.second - dataRange.first;
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
			auto keyOffset = plotRange.upper - dataRange.second;

			for (kIndex idx = 0; idx < data1d->count(); idx++) {
				auto val = data1d->value(idx);
				auto key = val.first + keyOffset;
				if (key < plotRange.lower)
					continue;

				keys.push_back(key);
				values.push_back(val.second);
			}

			graph->setData(keys, values);
		}
		else {
			graph->data()->clear();
			for (kIndex idx = 0; idx < data1d->count(); idx++) {
				auto val = data1d->value(idx);
				graph->addData(val.first, val.second);
			}
		}

		return true;
	}

}


KvPropertiedObject::kPropertySet KcDataPlot::propertySet() const
{
	using namespace kPrivate;

	kPropertySet ps;

	KpProperty prop;
	KpProperty subProp;

	prop.id = k_background;
	prop.name = QStringLiteral("Background");
	prop.flag = 0;
	prop.val = back_;
	ps.push_back(prop);

	if(type() == KePlot1dType::k_line)
		ps.push_back(lineProperty_(false));
	else if(type() == KePlot1dType::k_bars)
		ps.push_back(barProperty_());


	if (type() != KePlot1dType::k_bars)
		ps.push_back(scatterProperty_(type() == KePlot1dType::k_line));

	ps.push_back(axisProperty_());

	return ps;
}


KvPropertiedObject::KpProperty KcDataPlot::scatterProperty_(bool hasNone) const
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

	if (type() == KePlot1dType::k_line) {
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
	prop.children.push_back(subProp);

    return prop;
}

KvPropertiedObject::KpProperty KcDataPlot::lineProperty_(bool hasNone) const
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

KvPropertiedObject::KpProperty KcDataPlot::barProperty_() const
{
	KpProperty prop;
	KpProperty subProp;

	auto bars = dynamic_cast<QCPBars*>(customPlot_->plottable());
	assert(bars);

	prop.id = KvPropertiedObject::kInvalidId;
	prop.name = QStringLiteral("Bar");
	prop.flag = KvPropertiedObject::k_collapsed;

	// bar的绘图宽度，相对于band宽的ratio值，取值从1-100
	subProp.id = kPrivate::k_bar_width;
	subProp.name = "Width";
	subProp.val = QVariant::fromValue<int>(bars->width() * 100);
	subProp.minVal = 0;
	subProp.maxVal = 100;
	subProp.step = 1;
	prop.children.push_back(subProp);


	subProp.id = kPrivate::k_bar_pen;
	subProp.name = QStringLiteral("Pen");
	subProp.val = customPlot_->plottable()->pen();
	subProp.attr.penFlags = KvPropertiedObject::k_pen_all;
	prop.children.push_back(subProp);

	subProp.id = kPrivate::k_bar_fill_brush;
	subProp.name = QStringLiteral("Brush");
	subProp.val = QVariant::fromValue(customPlot_->plottable()->brush());
	//subProp.attr.showAllBrushStyle = true;
	prop.children.push_back(subProp);

	
	subProp.id = kPrivate::k_bar_fill_color;
	subProp.name = QStringLiteral("Color");
	subProp.val = QVariant::fromValue(customPlot_->plottable()->brush().color());
	subProp.attr.colorFlags = KvPropertiedObject::k_show_color_items | KvPropertiedObject::k_show_alpha_channel;
	prop.children.push_back(subProp);


	return prop;
}

KvPropertiedObject::KpProperty KcDataPlot::axisProperty_() const
{
	KpProperty prop;
	KpProperty subProp;

	prop.id = KvPropertiedObject::kInvalidId;
	prop.name = QStringLiteral("Axis");
	prop.val.clear();
	prop.flag = KvPropertiedObject::k_collapsed;

	if (!dynamic_cast<KvDataProvider*>(parent())->isStream()) {
		subProp.id = kPrivate::k_auto_scale;
		subProp.name = QStringLiteral("Auto-scale");
		subProp.disp = QStringLiteral("Auto scale");
		subProp.val = autoScale_;
		prop.children.push_back(subProp);
	}

	subProp.id = KvPropertiedObject::kInvalidId;
	subProp.name = QStringLiteral("Bottom");
	subProp.disp.clear();
	subProp.val.clear();
	subProp.flag = KvPropertiedObject::k_collapsed;
	subProp.children = kPrivate::getAxisProperties(customPlot_->xAxis, kPrivate::k_axis_bottom);
	prop.children.push_back(subProp);

	subProp.name = QStringLiteral("Left");
	subProp.children = kPrivate::getAxisProperties(customPlot_->yAxis, kPrivate::k_axis_left);
	prop.children.push_back(subProp);

	subProp.name = QStringLiteral("Top");
	subProp.children = kPrivate::getAxisProperties(customPlot_->xAxis2, kPrivate::k_axis_top);
	prop.children.push_back(subProp);

	subProp.name = QStringLiteral("Right");
	subProp.children = kPrivate::getAxisProperties(customPlot_->yAxis2, kPrivate::k_axis_right);
	prop.children.push_back(subProp);

	return prop;
}


void KcDataPlot::onPropertyChanged(int id, const QVariant& newVal)
{
	assert(id >= 0);

	using namespace kPrivate;

	if (id >= k_axis_bottom && id - k_axis_bottom <= k_axis_max)
		onAxisPropertyChanged(customPlot_->xAxis, id - k_axis_bottom, newVal);
	else if (id >= k_axis_left && id - k_axis_left <= k_axis_max)
		onAxisPropertyChanged(customPlot_->yAxis, id - k_axis_left, newVal);
	else if (id >= k_axis_top && id - k_axis_top <= k_axis_max)
		onAxisPropertyChanged(customPlot_->xAxis2, id - k_axis_top, newVal);
	else if (id >= k_axis_right && id - k_axis_right <= k_axis_max)
		onAxisPropertyChanged(customPlot_->yAxis2, id - k_axis_right, newVal);
	else {
		switch (id) {
		case k_auto_scale:
			autoScale_ = newVal.toBool();
			if (autoScale_) {
				rescaleAxes();

				// TODO: 同步axis的range属性
			}
			else {
				return; // no replot
			}
			break;

		case k_background:
			back_ = newVal.value<QColor>();
			customPlot_->setBackground(QBrush(back_));
			break;

		case k_line_pen:
		case k_bar_pen:
			customPlot_->plottable()->setPen(newVal.value<QPen>());
			break;

		case k_scatter_shape: {
			auto style = customPlot_->graph()->scatterStyle();
			style.setShape(QCPScatterStyle::ScatterShape(newVal.toInt()));
			customPlot_->graph()->setScatterStyle(style);
		    }
			break;

		case k_scatter_size: {
			auto style = customPlot_->graph()->scatterStyle();
			style.setSize(newVal.toDouble());
			customPlot_->graph()->setScatterStyle(style);
		    }
			break;

		case k_scatter_pen: {
			auto style = customPlot_->graph()->scatterStyle();
			style.setPen(newVal.value<QPen>());
			customPlot_->graph()->setScatterStyle(style);
		    }
		break;

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
		    }
			break;

		case k_bar_fill_color: {
			auto brush = customPlot_->plottable()->brush();
			brush.setColor(newVal.value<QColor>());
			customPlot_->plottable()->setBrush(brush);
		    }
			break;

		case k_bar_width: {
			auto bars = dynamic_cast<QCPBars*>(customPlot_->plottable());
			bars->setWidth(0.01 * newVal.toInt());
		    }
			break;
		}
	}

	if (customPlot_->isVisible()) 	
		customPlot_->replot();
}


QString KcDataPlot::exportAs()
{
	auto filter = QWidget::tr("export file type(*.pdf *.png *.jpg *.bmp)");
	auto path = QFileDialog::getSaveFileName(customPlot_, QWidget::tr("export plot"), "", filter);
	if (!path.isEmpty()) {

		auto ext = KuPathUtil::extension(path.toStdString(), false);
		if (::stricmp(ext.c_str(), "png") == 0)
			customPlot_->savePng(path);
		else if (::stricmp(ext.c_str(), "jpg") == 0)
			customPlot_->saveJpg(path);
		else if (::stricmp(ext.c_str(), "bmp") == 0)
			customPlot_->saveBmp(path);
		else
			customPlot_->savePdf(path);
	}

	return path;
}

/*
void KcDataPlot::clearData_(QCPAbstractPlottable* plot)
{
	if (kPrivate::clearPlotData<QCPGraph>(plot))
		return;

	if (kPrivate::clearPlotData<QCPBars>(plot))
		return;

	if (kPrivate::clearPlotData<QCPCurve>(plot))
		return;

	if (kPrivate::clearPlotData<QCPFinancial>(plot))
		return;

	if (kPrivate::clearPlotData<QCPStatisticalBox>(plot))
		return;

	if (kPrivate::clearPlotData<QCPColorMap>(plot))
		return;

	if (kPrivate::clearPlotData<QCPErrorBars>(plot))
		return;

	assert(false);
}
*/


bool KcDataPlot::render(std::shared_ptr<KvData> data)
{
	auto plot = customPlot_->plottable();

	assert(data->dim() == 1);
	auto data1d = std::dynamic_pointer_cast<KvData1d>(data);
	assert(data1d->channels() == 1); // TODO: 暂且假定单通道数据

	auto prov = dynamic_cast<KvDataProvider*>(parent());

	if (type() == KePlot1dType::k_scatter || type() == KePlot1dType::k_line) {
		kPrivate::plotData1d<QCPGraph>(plot, data1d, prov->isStream());
	}
	else if (type() == KePlot1dType::k_bars) {
		kPrivate::plotData1d<QCPBars>(plot, data1d, prov->isStream());

		auto xrange = prov->range(0);
		auto bars = dynamic_cast<QCPBars*>(plot);
		auto axisRange = static_cast<double>(xrange.second - xrange.first);
		auto bandNum = data1d->count();
		auto widthRatio = bars->width();
		bars->setWidth(axisRange / bandNum * widthRatio);
	}

	kAppEventHub->slotShowInDock(this, customPlot_);

	customPlot_->replot(prov->isStream() 
		? QCustomPlot::rpQueuedRefresh 
		: QCustomPlot::rpRefreshHint);

	return true;
}


void KcDataPlot::rescaleAxes()
{
	auto objp = dynamic_cast<KvDataProvider*>(parent());
	assert(objp);

	auto xrange = objp->range(0);
	auto yrange = objp->range(1);
	customPlot_->xAxis->setRange(xrange.first, xrange.second);
	customPlot_->yAxis->setRange(yrange.first, yrange.second);
	customPlot_->xAxis2->setRange(xrange.first, xrange.second);
	customPlot_->yAxis2->setRange(yrange.first, yrange.second);
}
