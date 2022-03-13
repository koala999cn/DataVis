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
}


KvCustomPlot::~KvCustomPlot()
{
	customPlot_->setParent(nullptr); // NOTE: 这很重要
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
