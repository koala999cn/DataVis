#include "KvRdCustomPlot.h"
#include "prov/KvDataProvider.h"
#include "base/KuPathUtil.h"
#include <QFileDialog>
#include "QtAppEventHub.h"
#include "qcustomplot/qcustomplot.h"
#include "gui/QtDataView.h"
#include "theme/KsThemeManager.h"
#include "theme/KcThemedQCP.h"


KvRdCustomPlot::KvRdCustomPlot(KvDataProvider* is, const QString& name)
	: KvDataRender(name, is)
{
	options_ = k_show;

	customPlot_ = new QCustomPlot(nullptr);
	rescaleAxes(); // 设置缺省的坐标轴范围
	autoScale_ = !is->isStream();

	// 设置上下文菜单
	customPlot_->setContextMenuPolicy(Qt::CustomContextMenu);
	customPlot_->connect(customPlot_, &QWidget::customContextMenuRequested, [this](const QPoint& pos) {
		QMenu menu(customPlot_);
		menu.addAction(tr("export..."), [this] { exportAs(); });
		menu.addSeparator();
		menu.addAction(tr("detail"), [this] { showData(); });
		menu.exec(customPlot_->mapToGlobal(pos));
		});

	themedPlot_ = std::make_unique<KcThemedQCP>(std::shared_ptr<QCustomPlot>(customPlot_));
}


KvRdCustomPlot::~KvRdCustomPlot()
{
	setOption(k_show, false);
	// delete customPlot_; 由themedPlot_管理
}


bool KvRdCustomPlot::getOption(KeObjectOption opt) const
{
	assert(opt == k_show);
	return customPlot_->isVisible();
}


void KvRdCustomPlot::setOption(KeObjectOption opt, bool on)
{
	assert(opt == k_show);
	if (on)
		kAppEventHub->showDock(this, customPlot_);
	else
		kAppEventHub->closeDock(this);
}


QString KvRdCustomPlot::exportAs()
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


void KvRdCustomPlot::showData()
{
	auto view = new QtDataView();
	auto plot = customPlot_->plottable(); // TODO: 使用tab支持多plot数据显示
	if (dynamic_cast<QCPGraph*>(plot)) 
		view->fill(*dynamic_cast<QCPGraph*>(plot)->data());
	else if (dynamic_cast<QCPBars*>(plot))
		view->fill(*dynamic_cast<QCPBars*>(plot)->data());
	else if (dynamic_cast<QCPColorMap*>(plot))
		view->fill(*dynamic_cast<QCPColorMap*>(plot)->data());

	// NOTO: 此处无法再用dock，因为this对象已关联到plot-dock
	// TODO: 可考虑一个obj关联多个dock的设计
	view->setWindowFlag(Qt::Window);
	view->setAttribute(Qt::WA_DeleteOnClose);
	view->show();
}


void KvRdCustomPlot::rescaleAxes()
{
	auto objp = dynamic_cast<KvDataProvider*>(parent());
	assert(objp);

	auto xrange = objp->range(0);
	auto yrange = objp->range(1);
	customPlot_->xAxis->setRange(xrange.low(), xrange.high());
	customPlot_->yAxis->setRange(yrange.low(), yrange.high());
	customPlot_->xAxis2->setRange(xrange.low(), xrange.high());
	customPlot_->yAxis2->setRange(yrange.low(), yrange.high());
}


namespace kPrivate
{
	enum KePlotBaseProperty
	{
		k_axis_bottom,
		k_axis_bottom_visible = k_axis_bottom,
		k_axis_bottom_range,
		k_axis_bottom_log,
		k_axis_bottom_tick_length,
		k_axis_bottom_subtick_length,
		k_axis_left,
		k_axis_left_visible = k_axis_left,
		k_axis_left_range,
		k_axis_left_log,
		k_axis_left_tick_length,
		k_axis_left_subtick_length,
		k_axis_top,
		k_axis_top_visible = k_axis_top,
		k_axis_top_range,
		k_axis_top_log,
		k_axis_top_tick_length,
		k_axis_top_subtick_length,
		k_axis_right,
		k_axis_right_visible = k_axis_right,
		k_axis_right_range,
		k_axis_right_log,
		k_axis_right_tick_length,
		k_axis_right_subtick_length,

		k_theme,
		k_canvas,
		k_palette,
		k_layout,
		k_background,
		k_margins,
		k_xrange,
		k_yrange,
		k_x_subtitle,
		k_y_subtitle,

		k_axis_visible = k_axis_bottom_visible - k_axis_bottom,
		k_axis_range = k_axis_bottom_range - k_axis_bottom,
		k_axis_log = k_axis_bottom_log - k_axis_bottom,
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
		prop.name = QStringLiteral("Visible");
		prop.flag = 0;
		prop.val = axis->visible();
		psAxis.push_back(prop);

		prop.id = idBase + k_axis_range;
		prop.name = QStringLiteral("Range");
		prop.flag = KvPropertiedObject::k_restrict;
		prop.val = QPointF(axis->range().lower, axis->range().upper);
		KvPropertiedObject::KpProperty subProp;
		subProp.name = QStringLiteral("low");
		prop.children.push_back(subProp);
		subProp.name = QStringLiteral("high");
		prop.children.push_back(subProp);
		psAxis.push_back(prop);

		prop.id = idBase + k_axis_log;
		prop.name = QStringLiteral("log");
		prop.disp = QStringLiteral("Log Scale");
		prop.val = axis->stLogarithmic == axis->scaleType();
		psAxis.push_back(prop);

		prop.id = idBase + k_axis_tick_length;
		prop.name = QStringLiteral("tick-length");
		prop.disp = QStringLiteral("Tick Length");
		prop.flag = 0;
		prop.val = QPoint(axis->tickLengthIn(), axis->tickLengthOut());
		subProp.name = QStringLiteral("in");
		prop.children.push_back(subProp);
		subProp.name = QStringLiteral("out");
		prop.children.push_back(subProp);
		psAxis.push_back(prop);

		prop.id = idBase + k_axis_subtick_length;
		prop.name = QStringLiteral("subtick-length");
		prop.disp = QStringLiteral("Subtick Length");
		prop.val = QPoint(axis->subTickLengthIn(), axis->subTickLengthOut());
		psAxis.push_back(prop);

		return psAxis;
	}

	static void onAxisPropertyChanged(QCPAxis* axis, int idDiff, const QVariant& newVal)
	{
		switch (idDiff) {
		case k_axis_visible:
			axis->setVisible(newVal.toBool());
			break;

		case k_axis_range:
			axis->setRange(newVal.toPointF().x(), newVal.toPointF().y());
			break;

		case k_axis_log:
			axis->setScaleType(newVal.toBool() ? axis->stLogarithmic : axis->stLinear);
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
}


KvRdCustomPlot::kPropertySet KvRdCustomPlot::propertySet() const
{
	using namespace kPrivate;

	kPropertySet ps;

	KpProperty prop, subProp;

	// 加载theme
	kThemeManager->load("themes/*.json"); // TODO: 移动KsThemeManager构造函数中去
	if(!kThemeManager->listThemes().empty())
	    ps.push_back(themeProperty_());

	/*
	prop.id = k_background;
	prop.name = QStringLiteral("Background");
	prop.flag = 0;
	prop.enumList.clear();
	prop.val = back_; // TODO: QCP目前没有获取背景brush的方法
	prop.children.clear();
	ps.push_back(prop);*/

	auto xaixs = customPlot_->xAxis;
	prop.id = k_xrange;
	prop.flag = KvPropertiedObject::k_restrict;
	prop.name = "xrange";
	prop.disp = tr("Key Range");
	prop.val = QPointF(xaixs->range().lower, xaixs->range().upper);
	subProp.name = tr("low");
	prop.children.push_back(subProp);
	subProp.name = tr("high");
	prop.children.push_back(subProp);
	ps.push_back(prop);

	auto yaixs = customPlot_->yAxis;
	prop.id = k_yrange;
	prop.name = "yrange";
	prop.disp = tr("Value Range");
	prop.val = QPointF(yaixs->range().lower, yaixs->range().upper);
	ps.push_back(prop);


	prop.id = KvPropertiedObject::kInvalidId;
	prop.name = "subtitle";
	prop.disp = tr("Axis title");
	prop.val.clear();
	prop.children.clear();

	subProp.id = k_x_subtitle;
	subProp.name = "x";
	subProp.val = xaixs->label();
	prop.children.push_back(subProp);

	subProp.id = k_y_subtitle;
	subProp.name = "y";
	subProp.val = yaixs->label();
	prop.children.push_back(subProp);

	ps.push_back(prop);


/*	prop.id = KvPropertiedObject::kInvalidId;
	prop.name = QStringLiteral("Axis");
	prop.val.clear();
	prop.flag = KvPropertiedObject::k_collapsed;

	subProp.id = KvPropertiedObject::kInvalidId;
	subProp.flag = KvPropertiedObject::k_collapsed;
	subProp.name = QStringLiteral("Bottom");
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
	ps.push_back(prop);*/

	/*
	auto grid = customPlot_->plotLayout();
	auto ele = grid->elementAt(0);
	auto m = ele->minimumMargins();
	prop.id = k_margins;
	prop.name = QStringLiteral("Margins");
	prop.val = QRect(m.left(), m.top(), m.right(), m.bottom());
	ps.push_back(prop);*/

	return ps;
}


void KvRdCustomPlot::setPropertyImpl_(int id, const QVariant& newVal)
{
	assert(id >= 0);

	using namespace kPrivate;

	switch (id) {
	case k_theme:
		themeName_ = kThemeManager->listThemes()[newVal.toInt()];
		applyTheme_(themeName_);
		break;

	case k_canvas: 
		canvasName_ = kThemeManager->listCanvas()[newVal.toInt()];
		kThemeManager->applyCanvas(canvasName_, themedPlot_.get());
		break;

	case k_palette: 
		paletteName_ = kThemeManager->listPalettes()[newVal.toInt()];
		kThemeManager->applyPalette(paletteName_, themedPlot_.get());
		break;

	case k_layout: 
		layoutName_ = kThemeManager->listLayouts()[newVal.toInt()];
		kThemeManager->applyLayout(layoutName_, themedPlot_.get());
		break;

	case k_background:
		themedPlot_->applyFill(KvThemedPlot::k_plot, newVal.value<QBrush>());
		break;

	case k_xrange:
		customPlot_->xAxis->setRange(newVal.toPointF().x(), newVal.toPointF().y());
		break;

	case k_yrange:
		customPlot_->yAxis->setRange(newVal.toPointF().x(), newVal.toPointF().y());
		break;

	case k_x_subtitle:
		customPlot_->xAxis->setLabel(newVal.toString());
		break;

	case k_y_subtitle:
		customPlot_->yAxis->setLabel(newVal.toString());
		break;

	case k_margins: {
		auto r = newVal.value<QRect>(); 
		// TODO:
		themedPlot_->applyMargins(KvThemedPlot::k_plot, { r.left(), r.top(), r.right(), r.bottom() });
		break;
	}

	default:
		if (id >= k_axis_bottom && id - k_axis_bottom <= k_axis_max) {
			onAxisPropertyChanged(customPlot_->xAxis, id - k_axis_bottom, newVal);
		}
		else if (id >= k_axis_left && id - k_axis_left <= k_axis_max) {
			onAxisPropertyChanged(customPlot_->yAxis, id - k_axis_left, newVal);
		}
		else if (id >= k_axis_top && id - k_axis_top <= k_axis_max) {
			onAxisPropertyChanged(customPlot_->xAxis2, id - k_axis_top, newVal);
		}
		else if (id >= k_axis_right && id - k_axis_right <= k_axis_max) {
			onAxisPropertyChanged(customPlot_->yAxis2, id - k_axis_right, newVal);
		}
		break;
	}
}


void KvRdCustomPlot::applyTheme_(const QString& name)
{
	themeName_ = name;

	auto canvas = kThemeManager->canvasName(name);
	auto idx = kThemeManager->listCanvas().indexOf(canvas);
	if (idx != -1) {
		canvasName_ = canvas;
		kAppEventHub->objectPropertyChanged(this, kPrivate::k_canvas, idx);
	}

	auto layout = kThemeManager->layoutName(name);
	idx = kThemeManager->listLayouts().indexOf(layout);
	if (idx != -1) {
		layoutName_ = layout;
		kAppEventHub->objectPropertyChanged(this, kPrivate::k_layout, idx);
	}

	auto palette = kThemeManager->paletteName(name);
	idx = kThemeManager->listPalettes().indexOf(palette);
	if (idx != -1) {
		paletteName_ = palette;
		kAppEventHub->objectPropertyChanged(this, kPrivate::k_palette, idx);
	}

	kThemeManager->applyTheme(name, themedPlot_.get());

	//kAppEventHub->refreshPropertySheet(); // TODO: 可能crack. 比如有delegate没有及时销毁，再访问的时候可能出错
}


KvRdCustomPlot::KpProperty KvRdCustomPlot::themeProperty_() const
{
	KpProperty prop;

	auto list = kThemeManager->listThemes();
	if (!list.empty()) {
		int idx = list.indexOf(themeName_);
		if (idx == -1) {
			idx = 0;
			kThemeManager->applyTheme(list.front(), themedPlot_.get());
		}
		prop.id = kPrivate::k_theme;
		prop.name = tr("Theme");
		prop.flag = 0;
		prop.val = idx;
		prop.makeEnum(list);
		prop.children.clear();

		KpProperty subProp;
		list = kThemeManager->listCanvas();
		if (!list.empty()) {
			int idx = list.indexOf(canvasName_);
			subProp.id = kPrivate::k_canvas;
			subProp.name = tr("Canvas");
			subProp.flag = 0;
			subProp.val = idx;
			subProp.makeEnum(list);
			prop.children.push_back(subProp);
		}

		list = kThemeManager->listPalettes();
		if (!list.empty()) {
			int idx = list.indexOf(paletteName_);
			subProp.id = kPrivate::k_palette;
			subProp.name = tr("Palette");
			subProp.flag = 0;
			subProp.val = idx;
			subProp.makeEnum(list);
			prop.children.push_back(subProp);
		}

		list = kThemeManager->listLayouts();
		if (!list.empty()) {
			int idx = list.indexOf(layoutName_);
			subProp.id = kPrivate::k_layout;
			subProp.name = tr("Layout");
			subProp.flag = 0;
			subProp.val = idx;
			subProp.makeEnum(list);
			prop.children.push_back(subProp);
		}
	}

	return prop;
}