#include "KgPlotTheme.h"
#include "KuPathUtil.h"
#include "KuThemeUtil.h"
#include <QFile>
#include <QBrush>
#include <QJsonDocument>
#include <assert.h>
#include <functional>
#include "qcustomplot/qcustomplot.h"


bool KgPlotTheme::load(const char* path)
{
	themes_.clear();
	int unnamed(0);

	auto files = KuPathUtil::getFiles(path, false, true);
	for (auto fileName : files) {
		QFile f(QString::fromStdString(fileName));
		if (!f.open(QIODevice::ReadOnly))
			continue;

		auto jdoc = QJsonDocument::fromJson(f.readAll());
		auto obj = jdoc.object();

		tryPlot_(obj, unnamed);

		if (obj.contains("themes") && obj["themes"].isArray()) {
			auto aobj = obj["themes"].toArray();
			for (auto iter = aobj.cbegin(); iter != aobj.cend(); ++iter) {
				auto obj = *iter;
				if (obj.isObject())
					tryPlot_(obj.toObject(), unnamed);
			}
		}
	}

	removeInvalidThemes_();

	return !themes_.empty();
}


void KgPlotTheme::tryPlot_(const QJsonObject& obj, int& unnamed)
{
	if (obj.contains("plot") && obj["plot"].isObject()) {
		QString name;
		if (obj.contains("name") && obj["name"].isString())
			name = obj["name"].toString();
		else
			name = QString("_unnamed%1_").arg(unnamed++);

		while (themes_.count(name) > 0)
			name += '_';

		themes_.emplace(std::move(name), obj["plot"].toObject());
	}
}


void KgPlotTheme::removeInvalidThemes_()
{
	// TODO:
}


QStringList KgPlotTheme::list() const
{
	QStringList list;
	for (auto i : themes_)
		list << i.first;

	return list;
}


void KgPlotTheme::apply(const QString& theme, QCustomPlot* plot) const
{
	if (!themes_.count(theme))
		return;

	auto& jobj = themes_.at(theme);

	tryBase_(jobj, plot);
	tryGlobal_(jobj, plot);
	tryBkgnd_(jobj, plot);
	tryAxes_(jobj, plot);
	tryGrid_(jobj, plot);
	tryData_(jobj, plot);
	tryText_(jobj, plot);
	tryMargins_(jobj, plot);
	tryLegend_(jobj, plot);
	trySpacing_(jobj, plot);
}


void KgPlotTheme::tryBase_(const QJsonObject& jobj, QCustomPlot* plot) const
{
	if (jobj.contains("base") && jobj["base"].isString()) 
		apply(jobj["base"].toString(), plot);
}


void KgPlotTheme::tryBkgnd_(const QJsonObject& jobj, QCustomPlot* plot)
{
	if (jobj.contains("background") && jobj["background"].isObject()) {
		QBrush brush;
		KuThemeUtil::apply(jobj["background"].toObject(), brush);
		if(brush != QBrush())
		    plot->setBackground(brush);
	}
}


namespace kPrivate
{
	enum KeAxisFilter
	{
		k_axis_x = QCPAxis::atTop | QCPAxis::atBottom,
		k_axis_y = QCPAxis::atLeft | QCPAxis::atRight,
		k_axis_all = k_axis_x | k_axis_y
	};


	void for_axis(QCustomPlot* plot, int filter, std::function<void(QCPAxis*)> op)
	{
		for (auto rect : plot->axisRects())
			for (auto axis : rect->axes())
				if (axis->axisType() & filter)
				    op(axis);
	}

	void handle_special_axes(const QJsonObject& jobj, QCustomPlot* plot, int level, 
		std::function<void(const QJsonValue&, QCustomPlot*, int)> op)
	{
		if (level == kPrivate::k_axis_all) { // 允许x, y特化
			if (jobj.contains("x"))
				op(jobj["x"], plot, kPrivate::k_axis_x);

			if (jobj.contains("y"))
				op(jobj["y"], plot, kPrivate::k_axis_y);
		}

		if (level == kPrivate::k_axis_all || level == kPrivate::k_axis_x) { // 允许top, bottom特化
			if (jobj.contains("top"))
				op(jobj["top"], plot, QCPAxis::atTop);

			if (jobj.contains("bottom"))
				op(jobj["bottom"], plot, QCPAxis::atBottom);
		}

		if (level == kPrivate::k_axis_all || level == kPrivate::k_axis_y) { // 允许left, right特化
			if (jobj.contains("left"))
				op(jobj["left"], plot, QCPAxis::atLeft);

			if (jobj.contains("right"))
				op(jobj["right"], plot, QCPAxis::atRight);
		}
	}
}


void KgPlotTheme::tryGlobal_(const QJsonObject& jobj, QCustomPlot* plot)
{
	if (jobj.contains("line")) {
		auto line = jobj["line"];
		applyAxesLine_(line, plot, kPrivate::k_axis_all);
		applyGridLine_(line, plot, kPrivate::k_axis_all);

		// TODO: legend & data-line ?
	}

	if (jobj.contains("title")) {
		auto title = jobj["title"];
		applyAxesTitle_(title, plot, kPrivate::k_axis_all);

		// TODO: plot title & legend title
	}

	if (jobj.contains("label")) {
		auto label = jobj["label"];
		applyAxesLabel_(label, plot, kPrivate::k_axis_all);

		// TODO: legend label
	}

	if (jobj.contains("text")) {
		auto text = jobj["text"];
		applyAxesTitle_(text, plot, kPrivate::k_axis_all);
		applyAxesLabel_(text, plot, kPrivate::k_axis_all);

		// TODO: 
	}
}


void KgPlotTheme::tryAxes_(const QJsonObject& jobj, QCustomPlot* plot)
{
	if (!jobj.contains("axis")) return;
	
	auto axes = jobj["axis"];

	// 解析坐标轴区域的背景属性
	if (axes.isObject()) {
		auto jobj = axes.toObject();
		if (jobj.contains("background") && jobj["background"].isObject()) {
			QBrush brush;
			KuThemeUtil::apply(jobj["background"].toObject(), brush);
			if (brush != QBrush())
				for (auto rect : plot->axisRects())
					rect->setBackground(brush);
		}
	}

	// 解析每个坐标轴的属性
	applyAxes_(axes, plot, kPrivate::k_axis_all);
}


void KgPlotTheme::applyAxes_(const QJsonValue& jval, QCustomPlot* plot, int level)
{
	// 处理null
	if (jval.isNull()) {
		kPrivate::for_axis(plot, level, [](QCPAxis* axis) {
			axis->setVisible(false); });
	}
	
	if (!jval.isObject())
		return;

	auto jobj = jval.toObject();

	/// 首先处理公用属性

	if (jobj.contains("line"))
		applyAxesBaseline_(jobj["line"], plot, level);

	if (jobj.contains("baseline"))
		applyAxesBaseline_(jobj["baseline"], plot, level);

	if (jobj.contains("tick"))
		applyAxesTick_(jobj["tick"], plot, level);

	if (jobj.contains("subtick"))
		applyAxesSubtick_(jobj["subtick"], plot, level);

	if (jobj.contains("title"))
		applyAxesTitle_(jobj["title"], plot, level);

	if (jobj.contains("label"))
		applyAxesLabel_(jobj["label"], plot, level);


	/// 然后处理特化属性
	kPrivate::handle_special_axes(jobj, plot, level, applyAxes_);
}


void KgPlotTheme::applyAxesLine_(const QJsonValue& jval, QCustomPlot* plot, int level)
{
	applyAxesBaseline_(jval, plot, level);
	applyAxesTick_(jval, plot, level);
	applyAxesSubtick_(jval, plot, level);
}


void KgPlotTheme::applyAxesBaseline_(const QJsonValue& jval, QCustomPlot* plot, int level)
{
	if (jval.isNull()) {
		kPrivate::for_axis(plot, level, [](QCPAxis* axis) {
			axis->setBasePen(QPen(Qt::NoPen)); });
		return;
	}
	else if (jval.isObject()) {
		auto jobj = jval.toObject();
		kPrivate::for_axis(plot, level, [&jobj](QCPAxis* axis) {
			auto pen = axis->basePen();
			KuThemeUtil::apply(jobj, pen);
			axis->setBasePen(pen); 
			});

		kPrivate::handle_special_axes(jobj, plot, level, applyAxesBaseline_);
	}
}

void KgPlotTheme::applyAxesTick_(const QJsonValue& jval, QCustomPlot* plot, int level)
{
	if (jval.isNull()) {
		kPrivate::for_axis(plot, level, [](QCPAxis* axis) {
			axis->setTicks(false); });
		return;
	}
	else if (!jval.isObject())
		return;

	// 先设置为可见
	kPrivate::for_axis(plot, level, [](QCPAxis* axis) {
		axis->setTicks(true);
		});

	auto jobj = jval.toObject();

	kPrivate::for_axis(plot, level, [&jobj](QCPAxis* axis) {
		auto pen = axis->tickPen();
		KuThemeUtil::apply(jobj, pen);
		axis->setTickPen(pen); 
		});

	int ilenght, olength;
	if (KuThemeUtil::tryInt(jobj, "inside", ilenght))
		kPrivate::for_axis(plot, level, [ilenght](QCPAxis* axis) {
		    axis->setTickLengthIn(ilenght); });
	if (KuThemeUtil::tryInt(jobj, "outside", olength))
		kPrivate::for_axis(plot, level, [olength](QCPAxis* axis) {
		    axis->setTickLengthOut(olength); });
	
	kPrivate::handle_special_axes(jobj, plot, level, applyAxesTick_);
}


void KgPlotTheme::applyAxesSubtick_(const QJsonValue& jval, QCustomPlot* plot, int level)
{
	if (jval.isNull()) {
		kPrivate::for_axis(plot, level, [](QCPAxis* axis) {
			axis->setSubTicks(false); });
		return;
	}
	else if (!jval.isObject())
		return;

	// 先设置为可见
	kPrivate::for_axis(plot, level, [](QCPAxis* axis) {
		axis->setSubTicks(true);
		});

	auto jobj = jval.toObject();

	kPrivate::for_axis(plot, level, [&jobj](QCPAxis* axis) {
		auto pen = axis->subTickPen();
		KuThemeUtil::apply(jobj, pen);
		axis->setSubTickPen(pen); 
		});


	int ilenght, olength;
	if (KuThemeUtil::tryInt(jobj, "inside", ilenght))
		kPrivate::for_axis(plot, level, [ilenght](QCPAxis* axis) {
		    axis->setSubTickLengthIn(ilenght); });
	if (KuThemeUtil::tryInt(jobj, "outside", olength))
		kPrivate::for_axis(plot, level, [olength](QCPAxis* axis) {
		    axis->setSubTickLengthOut(olength); });

	kPrivate::handle_special_axes(jobj, plot, level, applyAxesSubtick_);
}


void KgPlotTheme::applyAxesTitle_(const QJsonValue& jval, QCustomPlot* plot, int level)
{
	if (jval.isNull()) {
		kPrivate::for_axis(plot, level, [](QCPAxis* axis) {
			axis->setLabel(""); });
		return;
	}
	else if (!jval.isObject())
		return;

	auto jobj = jval.toObject();

	QColor color;
	if (KuThemeUtil::tryColor(jobj, "color", color))
		kPrivate::for_axis(plot, level, [color](QCPAxis* axis) {
		    axis->setLabelColor(color); });

	
	kPrivate::for_axis(plot, level, [&jobj](QCPAxis* axis) {
		QFont font = axis->labelFont();
		KuThemeUtil::apply(jobj, font);
		    axis->setLabelFont(font); 
		});

	kPrivate::handle_special_axes(jobj, plot, level, applyAxesTitle_);
}


void KgPlotTheme::applyAxesLabel_(const QJsonValue& jval, QCustomPlot* plot, int level)
{
	if (jval.isNull()) {
		kPrivate::for_axis(plot, level, [](QCPAxis* axis) {
			axis->setTickLabels(false); });
		return;
	}
	else if (!jval.isObject())
		return;

	// 先设置为可见
	kPrivate::for_axis(plot, level, [](QCPAxis* axis) {
		axis->setTickLabels(true);
		});

	auto jobj = jval.toObject();

	QColor color;
	if (KuThemeUtil::tryColor(jobj, "color", color))
		kPrivate::for_axis(plot, level, [color](QCPAxis* axis) {
		axis->setTickLabelColor(color); });


	kPrivate::for_axis(plot, level, [&jobj](QCPAxis* axis) {
		QFont font = axis->labelFont();
		KuThemeUtil::apply(jobj, font);
		axis->setTickLabelFont(font);
		});

	kPrivate::handle_special_axes(jobj, plot, level, applyAxesLabel_);

	if (jobj.contains("label") && jobj["label"].isObject()) {
		auto label = jobj["label"].toObject();
		QColor color;
		if (KuThemeUtil::tryColor(label, "color", color))
			kPrivate::for_axis(plot, level, [color](QCPAxis* axis) {
			axis->setTickLabelColor(color); });

		kPrivate::for_axis(plot, level, [&label](QCPAxis* axis) {
			auto font = axis->labelFont();
			KuThemeUtil::apply(label, font);
			axis->setTickLabelFont(font);
			});
	}
}


void KgPlotTheme::tryGrid_(const QJsonObject& jobj, QCustomPlot* plot)
{
	if (!jobj.contains("grid")) return;
	applyGrid_(jobj["grid"], plot, kPrivate::k_axis_all);
}


void KgPlotTheme::applyGrid_(const QJsonValue& jval, QCustomPlot* plot, int level)
{
	// 处理null
	if (jval.isNull()) {
		kPrivate::for_axis(plot, level, [](QCPAxis* axis) {
			axis->grid()->setVisible(false); });
		return;
	}

	if (!jval.isObject())
		return;

	// 先设置为可见
	kPrivate::for_axis(plot, level, [](QCPAxis* axis) {
		axis->grid()->setVisible(true);
		});

	auto jobj = jval.toObject();

	/// 首先处理公用属性

	// 全局画笔属性，允许两种设置方式：一是直接设置，二是通过line对象设置
	applyGridLine_(jval, plot, level); 
	if (jobj.contains("line"))
		applyGridLine_(jobj["line"], plot, level);

	if (jobj.contains("major"))
		applyGridMajor_(jobj["major"], plot, level);

	if (jobj.contains("minor"))
		applyGridMinor_(jobj["minor"], plot, level);

	if (jobj.contains("zeroline"))
		applyGridZeroline_(jobj["zeroline"], plot, level);


	/// 然后处理特化属性
	kPrivate::handle_special_axes(jobj, plot, level, applyGrid_);
}


void KgPlotTheme::applyGridLine_(const QJsonValue& jval, QCustomPlot* plot, int level)
{
	kPrivate::for_axis(plot, level, [&jval](QCPAxis* axis) {
		if (jval.isNull()) {
			QPen pen(Qt::NoPen);
			axis->grid()->setPen(pen);
			axis->grid()->setSubGridPen(pen);
			axis->grid()->setZeroLinePen(pen);
		}
		else if (jval.isObject()) {

			auto jobj = jval.toObject();

			auto pen = axis->grid()->pen();
			KuThemeUtil::apply(jobj, pen);
			axis->grid()->setPen(pen);

			pen = axis->grid()->subGridPen();
			KuThemeUtil::apply(jobj, pen);
			axis->grid()->setSubGridPen(pen);

			pen = axis->grid()->zeroLinePen();
			KuThemeUtil::apply(jobj, pen);
			axis->grid()->setZeroLinePen(pen);
		}
		});
}


void KgPlotTheme::applyGridMajor_(const QJsonValue& jval, QCustomPlot* plot, int level)
{
	if (jval.isNull()) {
		kPrivate::for_axis(plot, level, [](QCPAxis* axis) {
			axis->grid()->setVisible(false); });
		return;
	}
	else if (!jval.isObject())
		return;

	kPrivate::for_axis(plot, level, [](QCPAxis* axis) {
		axis->grid()->setVisible(true); });

	auto jobj = jval.toObject();

	kPrivate::for_axis(plot, level, [&jobj](QCPAxis* axis) {
		QPen pen = axis->grid()->pen();
		KuThemeUtil::apply(jobj, pen);
		axis->grid()->setPen(pen); 
		});

	kPrivate::handle_special_axes(jobj, plot, level, applyGridMajor_);
}


void KgPlotTheme::applyGridMinor_(const QJsonValue& jval, QCustomPlot* plot, int level)
{
	if (jval.isNull()) {
		kPrivate::for_axis(plot, level, [](QCPAxis* axis) {
			axis->grid()->setSubGridVisible(false); });
		return;
	}
	else if (!jval.isObject())
		return;

	kPrivate::for_axis(plot, level, [](QCPAxis* axis) {
		axis->grid()->setSubGridVisible(true); });

	auto jobj = jval.toObject();

	kPrivate::for_axis(plot, level, [&jobj](QCPAxis* axis) {
		QPen pen = axis->grid()->subGridPen();
		KuThemeUtil::apply(jobj, pen);
		axis->grid()->setSubGridPen(pen);
		});

	kPrivate::handle_special_axes(jobj, plot, level, applyGridMinor_);
}


void KgPlotTheme::applyGridZeroline_(const QJsonValue& jval, QCustomPlot* plot, int level)
{
	if (jval.isNull()) {
		kPrivate::for_axis(plot, level, [](QCPAxis* axis) {
			axis->grid()->setZeroLinePen(QPen(Qt::NoPen)); });
		return;
	}
	else if (!jval.isObject())
		return;

	// 设置为可见
	kPrivate::for_axis(plot, level, [](QCPAxis* axis) {
		auto pen = axis->grid()->zeroLinePen();
		if (pen == QPen(Qt::NoPen)) {
			pen = axis->grid()->pen();
			pen.setStyle(Qt::SolidLine);
			axis->grid()->setZeroLinePen(pen);
		}});

	auto jobj = jval.toObject();

	kPrivate::for_axis(plot, level, [&jobj](QCPAxis* axis) {
		QPen pen = axis->grid()->zeroLinePen();
		KuThemeUtil::apply(jobj, pen);
		axis->grid()->setZeroLinePen(pen);
		});

	kPrivate::handle_special_axes(jobj, plot, level, applyGridZeroline_);
}


void KgPlotTheme::tryData_(const QJsonObject& jobj, QCustomPlot* plot)
{

}


void KgPlotTheme::tryText_(const QJsonObject& jobj, QCustomPlot* plot)
{

}


void KgPlotTheme::tryMargins_(const QJsonObject& jobj, QCustomPlot* plot)
{
	if (jobj.contains("margins") && jobj["margins"].isObject()) {
		QMargins margins;
		KuThemeUtil::apply(jobj["margins"].toObject(), margins);
		auto layout = plot->plotLayout();
		for (int i = 0; i < layout->elementCount(); i++)
			layout->elementAt(i)->setMinimumMargins(margins);
	}
}


void KgPlotTheme::tryLegend_(const QJsonObject& jobj, QCustomPlot* plot)
{

}


void KgPlotTheme::trySpacing_(const QJsonObject& jobj, QCustomPlot* plot)
{

}
