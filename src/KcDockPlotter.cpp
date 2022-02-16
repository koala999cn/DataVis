#include "KcDockPlotter.h"
#include <assert.h>
#include "KcDataSnapshot.h"
#include "KvData1d.h"
#include "QtAppEventHub.h"
#include "base/KuPathUtil.h"
#include <QFileDialog>


KcDockPlotter::KcDockPlotter(KvInputSource* is, KDDockWidgets::MainWindow* dockFrame)
	: KvOutputDevice(is->name(), k_plot)
	, input_(is)
	, dockFrame_(dockFrame)
	, type_(-1) // 缺省为不显示
{
	customPlot_ = new QCustomPlot(nullptr);
	assert(customPlot_->graphCount() == 0);
	customPlot_->setOpenGl(false); // TODO: QCustomPlot在Opengl模式下有bug，待修复。已提交20220208
	autoScale_ = true;

	// 根据style设置背景色???TODO: 是否需要？前景色呢？
	back_ = dockFrame->style()->standardPalette().color(QPalette::Base);
	customPlot_->setBackground(QBrush(back_));

	// 设置上下文菜单
	customPlot_->setContextMenuPolicy(Qt::CustomContextMenu);
	customPlot_->connect(customPlot_, &QWidget::customContextMenuRequested, [this](const QPoint& pos) {
		QAction exportAction(QWidget::tr("export"), customPlot_);
		customPlot_->connect(&exportAction, &QAction::triggered, [this]() { exportPlot(); });
		QMenu menu(customPlot_);
		menu.addAction(&exportAction);
		menu.exec(customPlot_->mapToGlobal(pos));
	    });
}


KcDockPlotter::~KcDockPlotter()
{
    auto dock = getDock_();
	if (dock) {
		dock->setWidget(nullptr);
		dock->close();
		delete dock;
	}

	delete customPlot_;
}


namespace kPrivate
{
	const static QString kPlotTypeStr[] = 
	{
	    u8"点线图",
	    u8"柱状图"
	};

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
		k_pen,

		k_axis_visible = k_axis_bottom_visible - k_axis_bottom,
		k_axis_range = k_axis_bottom_range - k_axis_bottom,
		k_axis_tick_length = k_axis_bottom_tick_length - k_axis_bottom,
		k_axis_subtick_length = k_axis_bottom_subtick_length - k_axis_bottom,
		k_axis_max = k_axis_subtick_length
	};


	// @idBase: 用来计算真实id的基数
	static KvPropertiedObject::kPropertySet getAxisProperties(QCPAxis* axis, int idBase)
	{
		using namespace kPrivate;
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

	template<typename PLOT_TYPE>
	bool clearPlotData(QCPAbstractPlottable* plot)
	{
		auto graph = dynamic_cast<PLOT_TYPE*>(plot);
		if (graph) {
			graph->data()->clear();
			return true;
		}

		return false;
	}

	template<typename PLOT_TYPE>
	bool shiftPlotData1d(QCPAbstractPlottable* plot, std::shared_ptr<KvData1d> data1d)
	{
		auto graph = dynamic_cast<PLOT_TYPE*>(plot);
		if (graph) {
			auto data = graph->data();

			auto dataRange = data1d->xrange();
			auto plotRange = plot->keyAxis()->range();
				
			auto offset = dataRange.second - dataRange.first;
			data->removeBefore(offset + plotRange.lower); // 前推已有数据

			QVector<double> keys, values;
			keys.reserve(data1d->count());
			values.reserve(data1d->count());
			if (offset < plotRange.upper - +plotRange.lower) { // 新数据比已有数据少, 须保留部分旧数据
				for (unsigned i = 0; i < data->size(); i++) { 
					auto iter = data->at(i);
					keys.push_back(iter->key - offset); // 修正残留数据的key值
					values.push_back(iter->value);
				}
			}
			
			// 新数据key值的偏移 
			offset = plotRange.upper - dataRange.second;

			for (kIndex idx = 0; idx < data1d->count(); idx++) {
				auto val = data1d->value(idx);
				auto key = val.first + offset;
				if (key < plotRange.lower)
					continue;
				keys.push_back(key);
				values.push_back(val.second);
			}
	
			graph->setData(keys, values);
			return true;
		}

		return false;
	}

	template<typename PLOT_TYPE>
	bool syncPlotData1d(QCPAbstractPlottable* plot, std::shared_ptr<KvData1d> data1d)
	{
		auto graph = dynamic_cast<PLOT_TYPE*>(plot);
		if (graph) {
			clearPlotData<PLOT_TYPE>(plot);
			for (kIndex idx = 0; idx < data1d->count(); idx++) {
				auto val = data1d->value(idx);
				graph->addData(val.first, val.second);
			}

			return true;
		}

		return false;
	}
}


int KcDockPlotter::supportedTypes()
{
	return sizeof(kPrivate::kPlotTypeStr) / sizeof(QString);
}


QStringList KcDockPlotter::supportedTypeStrs()
{
	QStringList sl;
	for (int i = 0; i < KcDockPlotter::supportedTypes(); i++) 
			sl.push_back(typeToStr(i));

	return sl;
}


QString KcDockPlotter::typeToStr(int type)
{
	assert(type >= 0 && type < supportedTypes());

	return kPrivate::kPlotTypeStr[type];
}


int KcDockPlotter::typeFromStr(const QString& str)
{
	for (int type = 0; type < supportedTypes(); type++)
		if (str == typeToStr(type))
			return type;

	return -1; // 未知
}


void KcDockPlotter::setName(const QString& newName)
{
	KvPropertiedObject::setName(newName);
	auto dock = getDock_();
	if (dock)
		dock->setTitle(displayName());
}


KvPropertiedObject::kPropertySet KcDockPlotter::propertySet() const
{
	using namespace kPrivate;

	kPropertySet ps;

	if (type_ < 0)
		return ps;

	KpProperty prop;
	prop.id = k_background;
	prop.name = QStringLiteral("Background");
	prop.flag = 0;
	prop.val = back_;
	ps.push_back(prop);

	prop.id = k_pen;
	prop.name = QStringLiteral("Pen");
	prop.val = customPlot_->plottable()->pen();
	prop.attr.penFlags = KvPropertiedObject::k_pen_all;
	ps.push_back(prop);


	prop.id = KvPropertiedObject::kInvalidId;
	prop.name = QStringLiteral("Axis");
	prop.val.clear();
	prop.flag = KvPropertiedObject::k_collapsed;

	KpProperty subProp;
	subProp.id = k_auto_scale;
	subProp.name = QStringLiteral("Auto-scale");
	subProp.disp = QStringLiteral("Auto scale");
	subProp.val = autoScale_;
	prop.children.push_back(subProp);
	if (autoScale_)
		customPlot_->rescaleAxes();

	subProp.id = KvPropertiedObject::kInvalidId;
	subProp.name = QStringLiteral("Bottom");
	subProp.disp.clear();
	subProp.val.clear();
	subProp.flag = KvPropertiedObject::k_collapsed;
	subProp.children = getAxisProperties(customPlot_->xAxis, k_axis_bottom);
	prop.children.push_back(subProp);

	subProp.name = QStringLiteral("Left");
	subProp.children = getAxisProperties(customPlot_->yAxis, k_axis_left);
	prop.children.push_back(subProp);

	subProp.name = QStringLiteral("Top");
	subProp.children = getAxisProperties(customPlot_->xAxis2, k_axis_top);
	prop.children.push_back(subProp);

	subProp.name = QStringLiteral("Right");
	subProp.children = getAxisProperties(customPlot_->yAxis2, k_axis_right);
	prop.children.push_back(subProp);
	ps.push_back(prop);

	return ps;
}


void KcDockPlotter::onPropertyChanged(int id, const QVariant& newVal)
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
				customPlot_->rescaleAxes();

				// TODO: 同步axis的range属性
			}
			break;

		case k_background:
			back_ = newVal.value<QColor>();
			customPlot_->setBackground(QBrush(back_));
			break;

		case k_pen:
			customPlot_->plottable()->setPen(newVal.value<QPen>());
		}
	}

	if (customPlot_->isVisible()) 	
		customPlot_->replot();
}


void KcDockPlotter::setType(int type)
{
	if (type == type_)
		return;

	//for (int i = 0; i < widget_->plottableCount(); i++) {
	//	auto p = widget_->plottable(i);
	//	clearData_(p);
	//}
	customPlot_->clearPlottables(); // TODO: 会丢失已设置plot的属性

	if (type == -1) { // 不显示
		auto dock = getDock_();
		if (dock) { // 关闭绘图dock
			dock->setWidget(nullptr);
			dock->close();
			delete dock;
		}		
	}
	else if(type == k_point_line) {
		auto graph = customPlot_->addGraph();
		graph->setAdaptiveSampling(true);

		if (input_->isStream()) {
			graph->addData(0, 0);
		}
	}
	else if (type == k_bar) {
		auto bars = new QCPBars(customPlot_->xAxis, customPlot_->yAxis);
	}

	type_ = type;

	// 同步显示
	if (customPlot_->isVisible())
		syncData();
}


bool KcDockPlotter::show()
{
	assert(type_ >= 0);

	auto dock = getDock_(true);
	if (dock == nullptr)
		return false;

	if (!dock->isVisible()) {
		syncData();
		dock->setWidget(customPlot_);
		dock->show();
	}
	dock->raise();

	return true;
}


KDDockWidgets::DockWidgetBase* KcDockPlotter::getDock_(bool createWhenNeeded)
{
	auto dockName = QString::number(reinterpret_cast<qlonglong>(input_), 16);
	auto dock = KDDockWidgets::DockWidget::byName(dockName);
	if (dock == nullptr && createWhenNeeded) {
		dock = new KDDockWidgets::DockWidget{ dockName };
		dock->setTitle(displayName());
		dock->setWidget(customPlot_);
		assert(customPlot_->parentWidget() == dock);
		dock->setAffinities({ "plot-dock" });
		dockFrame_->addDockWidget(dock, KDDockWidgets::Location_None);

		kAppEventHub->connect(dock, &KDDockWidgets::DockWidget::isFocusedChanged, kAppEventHub, [this](bool focus) {
			kAppEventHub->slotOutputDeviceActivated(this, focus);
			}
			);
	}
	else if(dock) {
		assert(dock->widget() == customPlot_);
		assert(dock->title() == displayName());
	}

	return dock;
}


void KcDockPlotter::syncData()
{
	assert(customPlot_->plottableCount() == 1);
	
	if (!input_->isStream()) {
		auto input = dynamic_cast<KcDataSnapshot*>(input_);
		assert(input);
		assert(input->data()->dim() == 1);
		auto data1d = std::dynamic_pointer_cast<KvData1d, KvData>(input->data());

		auto plot = customPlot_->plottable();
		if (type_ == k_point_line)
			kPrivate::syncPlotData1d<QCPGraph>(plot, data1d);
		else if (type_ == k_bar)
			kPrivate::syncPlotData1d<QCPBars>(plot, data1d);

		if (autoScale_)
			customPlot_->rescaleAxes();

		customPlot_->replot();
	}
}


QString KcDockPlotter::exportPlot()
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


void KcDockPlotter::clearData_(QCPAbstractPlottable* plot)
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


void KcDockPlotter::streamData(std::shared_ptr<KvData> data)
{
	if (type() == k_point_line || type() == k_bar) {
		auto plot = customPlot_->plottable();

		assert(data->dim() == 1);
		auto data1d = std::dynamic_pointer_cast<KvData1d>(data);
		assert(data1d->channels() == 1); // 暂且假定单通道数据

		if (type() == k_point_line)
			kPrivate::shiftPlotData1d<QCPGraph>(plot, data1d);
		else if (type() == k_bar)
			kPrivate::shiftPlotData1d<QCPBars>(plot, data1d);

		customPlot_->replot(QCustomPlot::rpQueuedRefresh);

		//if (kPrivate::shiftData1d<QCPCurve>(plot, buf, N))
		//	return;

		//if (kPrivate::shiftData1d<QCPFinancial>(plot, buf, N))
		//	return;

		//if (kPrivate::shiftData1d<QCPStatisticalBox>(plot, buf, N))
		//	return;

	}
}
