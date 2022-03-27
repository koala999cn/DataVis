#include "KvCustomPlot.h"
#include "KvDataProvider.h"
#include "base/KuPathUtil.h"
#include <QFileDialog>
#include "QtAppEventHub.h"
#include "qcustomplot/qcustomplot.h"


KvCustomPlot::KvCustomPlot(KvDataProvider* is, const QString& name)
	: KvDataRender(name, is)
{
	customPlot_ = new QCustomPlot(nullptr);
	rescaleAxes(); // 设置缺省的坐标轴范围
	autoScale_ = !is->isStream();

	// TODO: 根据style设置背景色
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
}


KvCustomPlot::~KvCustomPlot()
{
	show(false);
	delete customPlot_;
}



bool KvCustomPlot::canShown() const
{
	return true;
}


bool KvCustomPlot::isVisible() const
{
	return customPlot_->isVisible();
}


void KvCustomPlot::show(bool bShow)
{
	if (bShow)
		kAppEventHub->slotShowInDock(this, customPlot_);
	else
		kAppEventHub->slotCloseDock(this);
}


QString KvCustomPlot::exportAs()
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


void KvCustomPlot::rescaleAxes()
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


KvCustomPlot::kPropertySet KvCustomPlot::propertySet() const
{
	using namespace kPrivate;

	kPropertySet ps;

	KpProperty prop;

	prop.id = k_background;
	prop.name = QStringLiteral("Background");
	prop.flag = 0;
	prop.val = back_;
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


void KvCustomPlot::setPropertyImpl_(int id, const QVariant& newVal)
{
	assert(id >= 0);

	using namespace kPrivate;

	if (id == k_background) {
		back_ = newVal.value<QColor>();
		customPlot_->setBackground(QBrush(back_));
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

