#include "KvRdCustomPlot.h"
#include "prov/KvDataProvider.h"
#include "base/KuPathUtil.h"
#include <QFileDialog>
#include "QtAppEventHub.h"
#include "qcustomplot/qcustomplot.h"
#include "gui/QtDataView.h"
#include "theme/KgPlotTheme.h"


KvRdCustomPlot::KvRdCustomPlot(KvDataProvider* is, const QString& name)
	: KvDataRender(name, is)
{
	options_ = k_show;

	customPlot_ = new QCustomPlot(nullptr);
	rescaleAxes(); // 设置缺省的坐标轴范围
	autoScale_ = !is->isStream();

	// TODO: 根据style设置背景色
	back_ = QColor(255, 255, 255); // dockFrame->style()->standardPalette().color(QPalette::Base);
	customPlot_->setBackground(QBrush(back_));

	// 设置上下文菜单
	customPlot_->setContextMenuPolicy(Qt::CustomContextMenu);
	customPlot_->connect(customPlot_, &QWidget::customContextMenuRequested, [this](const QPoint& pos) {
		QMenu menu(customPlot_);
		menu.addAction(tr("export..."), [this] { exportAs(); });
		menu.addSeparator();
		menu.addAction(tr("detail"), [this] { showData(); });
		menu.exec(customPlot_->mapToGlobal(pos));
		});

	// 加载theme
	KgPlotTheme theme;
	theme.load("themes/*.json");
	theme.apply("dracula", customPlot_);
}


KvRdCustomPlot::~KvRdCustomPlot()
{
	setOption(k_show, false);
	delete customPlot_;
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

		k_background,
		k_margins,

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

	KpProperty prop;

	prop.id = k_background;
	prop.name = QStringLiteral("Background");
	prop.flag = 0;
	prop.val = back_;
	ps.push_back(prop);

	auto grid = customPlot_->plotLayout();
	auto ele = grid->elementAt(0);
	auto m = ele->minimumMargins();
	prop.id = k_margins;
	prop.name = QStringLiteral("Margins");
	prop.val = QRect(m.left(), m.top(), m.right(), m.bottom());
	ps.push_back(prop);

	prop.id = KvPropertiedObject::kInvalidId;
	prop.name = QStringLiteral("Axis");
	prop.val.clear();
	prop.flag = KvPropertiedObject::k_collapsed;

	KpProperty subProp;
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
	ps.push_back(prop);

	return ps;
}


void KvRdCustomPlot::setPropertyImpl_(int id, const QVariant& newVal)
{
	assert(id >= 0);

	using namespace kPrivate;

	if (id == k_background) {
		back_ = newVal.value<QColor>();
		customPlot_->setBackground(QBrush(back_));
	}
	else if (id == k_margins) {
		auto r = newVal.value<QRect>();
		auto grid = customPlot_->plotLayout();
		auto ele = grid->elementAt(0);
		ele->setMinimumMargins(QMargins(r.left(), r.top(), r.right(), r.bottom()));
	}
	else if (id >= k_axis_bottom && id - k_axis_bottom <= k_axis_max) {
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
}

