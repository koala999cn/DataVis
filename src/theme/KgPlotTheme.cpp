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
	canvas_.clear();
	palettes_.clear();
	layouts_.clear();

	auto files = KuPathUtil::getFiles(path, false, true);
	for (auto fileName : files) {
		QFile f(QString::fromStdString(fileName));
		if (!f.open(QIODevice::ReadOnly))
			continue;

		auto jdoc = QJsonDocument::fromJson(f.readAll());
		if (jdoc.isObject())
			tryLoad_(jdoc.object());
		else if (jdoc.isArray()) {
			auto ar = jdoc.array();
			for (auto i = ar.begin(); i != ar.end(); ++i)
				if (i->isObject()) // �����ǵݹ�����Ľṹ
					tryLoad_(i->toObject());
		}
	}

	removeInvalidThemes_();

	return !themes_.empty();
}


void KgPlotTheme::tryLoad_(const QJsonObject& jobj)
{
	if (jobj.contains("theme"))
		tryObjectOrArray_(jobj["theme"], [this](const QJsonObject& jobj) {
			if (jobj.contains("name") && jobj["name"].isString()) // ����δ����������
				themes_.emplace(std::move(jobj["name"].toString()), jobj);
			});

	if (jobj.contains("theme-list") && jobj["theme-list"].isObject())
		tryList_(jobj["theme-list"].toObject(), [this](const QString& key, const QJsonValue& jval) {
		    if(jval.isObject())
		        themes_.emplace(key, jval.toObject());
			});

	if (jobj.contains("canvas"))
		tryObjectOrArray_(jobj["canvas"], [this](const QJsonObject& jobj) {
			if (jobj.contains("name") && jobj["name"].isString()) 
				canvas_.emplace(jobj["name"].toString(), jobj);
			});

	if (jobj.contains("canvas-list") && jobj["canvas-list"].isObject())
		tryList_(jobj["canvas-list"].toObject(), [this](const QString& key, const QJsonValue& jval) {
		    canvas_.emplace(key, jval);
			});

	if (jobj.contains("palette"))
		tryObjectOrArray_(jobj["palette"], [this](const QJsonObject& jobj) {
		    if (jobj.contains("name") && jobj["name"].isString())
			    palettes_.emplace(jobj["name"].toString(), jobj);
			});

	if (jobj.contains("palette-list") && jobj["palette-list"].isObject())
		tryList_(jobj["palette-list"].toObject(), [this](const QString& key, const QJsonValue& jval) {
		    palettes_.emplace(key, jval);
			});

	if (jobj.contains("layout"))
		tryObjectOrArray_(jobj["layout"], [this](const QJsonObject& jobj) {
		    if (jobj.contains("name") && jobj["name"].isString())
			    layouts_.emplace(jobj["name"].toString(), jobj);
			});

	if (jobj.contains("layout-list") && jobj["layout-list"].isObject())
		tryList_(jobj["layout-list"].toObject(), [this](const QString& key, const QJsonValue& jval) {
		    if(jval.isObject())
		        layouts_.emplace(key, jval.toObject());
			});
}


void KgPlotTheme::tryObjectOrArray_(const QJsonValue& jval, std::function<void(const QJsonObject&)> fn)
{
	if (jval.isObject())
		fn(jval.toObject());
	else if (jval.isArray()) {
		auto ar = jval.toArray();
		for (auto i = ar.begin(); i != ar.end(); ++i)
			if (i->isObject())
				fn(i->toObject());
	}
}


void KgPlotTheme::tryList_(const QJsonObject& jobj, std::function<void(const QString& key, const QJsonValue&)> fn)
{
	for (auto i = jobj.begin(); i != jobj.end(); ++i)
	    fn(i.key(), *i);
}


void KgPlotTheme::removeInvalidThemes_()
{
	// TODO:
}


QStringList KgPlotTheme::listThemes() const
{
	QStringList list;
	for (auto i : themes_)
		list << i.first;

	return list;
}


QStringList KgPlotTheme::listCanvas() const
{
	QStringList list;
	for (auto i : canvas_)
		list << i.first;

	return list;
}


QStringList KgPlotTheme::listPalettes() const
{
	QStringList list;
	for (auto i : palettes_)
		list << i.first;

	return list;
}


QStringList KgPlotTheme::listLayouts() const
{
	QStringList list;
	for (auto i : layouts_)
		list << i.first;

	return list;
}


void KgPlotTheme::applyTheme(const QString& name, QCustomPlot* plot) const
{
	if (!themes_.count(name))
		return;

	auto& jobj = themes_.at(name);

	if (jobj.contains("base") && jobj["base"].isString())
		applyTheme(jobj["base"].toString(), plot);

	tryCanvas_(jobj, plot);
	tryPalette_(jobj, plot);
	tryLayout_(jobj, plot);
	applyLayout_(jobj, plot, true); // enable applyLayout_ to ignore "base" item
}


void KgPlotTheme::applyLayout(const QString& name, QCustomPlot* plot) const
{
	if (!layouts_.count(name))
		return;

	applyLayout_(layouts_.at(name), plot, false);
}


void KgPlotTheme::applyCanvas(const QString& name, QCustomPlot* plot) const
{
	if (!canvas_.count(name))
		return;

	applyCanvas_(canvas_.at(name), plot);
}


void KgPlotTheme::applyPalette(const QString& name, QCustomPlot* plot) const
{
	if (!palettes_.count(name))
		return;

	applyPalette_(palettes_.at(name), plot);
}


void KgPlotTheme::tryCanvas_(const QJsonObject& jobj, QCustomPlot* plot) const
{
	if (jobj.contains("canvas")) {
		auto canvas = jobj["canvas"];
		if (canvas.isString())
			applyCanvas(canvas.toString(), plot);
		else 
			applyCanvas_(canvas, plot);
	}
}


void KgPlotTheme::tryPalette_(const QJsonObject& jobj, QCustomPlot* plot) const
{
	if (jobj.contains("palette")) {
		auto palette = jobj["palette"];
		if (palette.isString())
			applyPalette(palette.toString(), plot);
		else 
			applyPalette_(palette, plot);
	}
}


void KgPlotTheme::tryLayout_(const QJsonObject& jobj, QCustomPlot* plot) const
{
	if (jobj.contains("layout")) {
		auto layout = jobj["layout"];
		if (layout.isString())
			applyLayout(layout.toString(), plot);
		else if (layout.isObject())
			applyLayout_(layout.toObject(), plot, false);
	}
}


void KgPlotTheme::applyLayout_(const QJsonObject& jobj, QCustomPlot* plot, bool inTheme) const
{
	if (!inTheme) { // try base
		if (jobj.contains("base") && jobj["base"].isString())
			applyLayout(jobj["base"].toString(), plot);
	}

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


void KgPlotTheme::applyCanvas_(const QJsonValue& jval, QCustomPlot* plot) const
{
	if (jval.isObject()) {
		auto jobj = jval.toObject();

		if (jobj.contains("background"))
			applyCanvasBkgnd_(jobj["background"], plot);

		if (jobj.contains("axis-rect"))
			applyCanvasAxisRect_(jobj["axis-rect"], plot);

		if (jobj.contains("text"))
			applyCanvasText_(jobj["text"], plot);

		if (jobj.contains("line"))
			applyCanvasLine_(jobj["line"], plot);

		if (jobj.contains("gridline"))
			applyCanvasGridline_(jobj["gridline"], plot);
	}
	else if (jval.isArray()) {
		auto ar = jval.toArray();
		auto sz = ar.size();

		if (sz > 0)
			applyCanvasBkgnd_(ar.at(0), plot);

		if (sz > 1)
			applyCanvasAxisRect_(ar.at(1), plot);

		if (sz > 2)
			applyCanvasText_(ar.at(2), plot);

		if (sz > 3)
			applyCanvasLine_(ar.at(3), plot);

		if (sz > 4)
			applyCanvasGridline_(ar.at(4), plot);
	}
}


void KgPlotTheme::applyCanvasBkgnd_(const QJsonValue& jval, QCustomPlot* plot)
{
	if (jval.isString()) {
		QColor color(jval.toString());
		plot->setBackground(QBrush(color));
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
		if (level == kPrivate::k_axis_all) { // ����x, y�ػ�
			if (jobj.contains("x"))
				op(jobj["x"], plot, kPrivate::k_axis_x);

			if (jobj.contains("y"))
				op(jobj["y"], plot, kPrivate::k_axis_y);
		}

		if (level == kPrivate::k_axis_all || level == kPrivate::k_axis_x) { // ����top, bottom�ػ�
			if (jobj.contains("top"))
				op(jobj["top"], plot, QCPAxis::atTop);

			if (jobj.contains("bottom"))
				op(jobj["bottom"], plot, QCPAxis::atBottom);
		}

		if (level == kPrivate::k_axis_all || level == kPrivate::k_axis_y) { // ����left, right�ػ�
			if (jobj.contains("left"))
				op(jobj["left"], plot, QCPAxis::atLeft);

			if (jobj.contains("right"))
				op(jobj["right"], plot, QCPAxis::atRight);
		}
	}


	void for_text(QCustomPlot* plot, std::function<QFont(const QFont&)> op)
	{
		plot->setFont(op(plot->font()));

		for (auto rect : plot->axisRects())
			for (auto axis : rect->axes()) {
				axis->setLabelFont(op(axis->labelFont()));
				axis->setTickLabelFont(op(axis->tickLabelFont()));
			}
	}


	void for_gridline(QCustomPlot* plot, std::function<QPen(const QPen&)> op)
	{
		for (auto rect : plot->axisRects())
			for (auto axis : rect->axes()) {
				auto grid = axis->grid();
				grid->setPen(op(grid->pen()));
				grid->setSubGridPen(op(grid->subGridPen()));
				grid->setZeroLinePen(op(grid->zeroLinePen()));
			}
	}


	void for_line(QCustomPlot* plot, std::function<QPen(const QPen&)> op)
	{
		for (auto rect : plot->axisRects())
			for (auto axis : rect->axes()) {
				axis->setBasePen(op(axis->basePen()));
				axis->setTickPen(op(axis->tickPen()));
				axis->setSubTickPen(op(axis->subTickPen()));
				
				auto grid = axis->grid();
				grid->setPen(op(grid->pen()));
				grid->setSubGridPen(op(grid->subGridPen()));
			}
	}
}

void KgPlotTheme::applyCanvasAxisRect_(const QJsonValue& jval, QCustomPlot* plot)
{
	if (jval.isString()) {
		QColor color(jval.toString());
		QBrush brush(color);
		kPrivate::for_axis(plot, kPrivate::k_axis_all, [&brush](QCPAxis* axis) {
			axis->axisRect()->setBackground(brush);
			});
	}
}


void KgPlotTheme::applyCanvasText_(const QJsonValue& jval, QCustomPlot* plot)
{
	if (jval.isString()) {
		QColor color(jval.toString());
		kPrivate::for_axis(plot, kPrivate::k_axis_all, [&color](QCPAxis* axis) {
			axis->setLabelColor(color);
			axis->setTickLabelColor(color);
			});

		// TODO: legend
	}
}


void KgPlotTheme::applyCanvasLine_(const QJsonValue& jval, QCustomPlot* plot)
{
	if (jval.isString()) {
		QColor color(jval.toString());
		kPrivate::for_line(plot, [&color](const QPen& pen) {
			QPen newPen(pen);
			newPen.setColor(color);
			return newPen;
			});
	}
}


void KgPlotTheme::applyCanvasGridline_(const QJsonValue& jval, QCustomPlot* plot)
{
	if (jval.isString()) {
		QColor color(jval.toString());
		kPrivate::for_gridline(plot, [&color](const QPen& pen) {
			QPen newPen(pen);
			newPen.setColor(color);
			return newPen;
			});
	}
}


void KgPlotTheme::applyPalette_(const QJsonValue& jval, QCustomPlot* plot) const
{
	if (jval.isArray()) {
		auto ar = jval.toArray();
		std::vector<QColor> colors;
		colors.reserve(ar.size());
		for (auto i = ar.begin(); i != ar.end(); ++i)
			if (i->isString())
				colors.push_back(QColor(i->toString()));

		for (int i = 0; i < plot->plottableCount(); i++) {
			auto p = plot->plottable(i);
			auto pen = p->pen();
			pen.setColor(colors[i % colors.size()]);
			p->setPen(pen);

			auto brush = p->brush();
			brush.setColor(colors[i % colors.size()]);
			p->setBrush(brush);
		}
	}
	else if (jval.isObject()) {
		// TODO:
	}
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

	// ��������������ı�������
	if (axes.isObject()) {
		auto jobj = axes.toObject();
		if (jobj.contains("background") && jobj["background"].isObject()) {
			QBrush brush;
			KuThemeUtil::apply(jobj["background"].toObject(), brush);
			if (brush != QBrush())
				for (auto rect : plot->axisRects())
					rect->setBackground(brush);
		}

		// Ĭ�ϴ�left & bottom������
		kPrivate::for_axis(plot, QCPAxis::atLeft | QCPAxis::atBottom, [](QCPAxis* axis) {
			axis->setVisible(true); });
	}

	// ����ÿ�������������
	applyAxes_(axes, plot, kPrivate::k_axis_all);
}


void KgPlotTheme::applyAxes_(const QJsonValue& jval, QCustomPlot* plot, int level)
{
	// ����null
	if (jval.isNull()) {
		kPrivate::for_axis(plot, level, [](QCPAxis* axis) {
			axis->setVisible(false); });
	}
	
	if (!jval.isObject())
		return;

	auto jobj = jval.toObject();

	/// ���ȴ�����������

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


	/// Ȼ�����ػ�����
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
			QPen pen = axis->basePen();
			pen.setStyle(Qt::NoPen);
			axis->setBasePen(pen); });
		return;
	}
	else if (jval.isObject()) {
		auto jobj = jval.toObject();
		kPrivate::for_axis(plot, level, [&jobj](QCPAxis* axis) {
			auto pen = axis->basePen();
			if (pen.style() == Qt::NoPen)
				pen.setStyle(Qt::SolidLine); // TODO:
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

	// ������Ϊ�ɼ�
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

	// ������Ϊ�ɼ�
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

	// ������Ϊ�ɼ�
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
	// ����null
	if (jval.isNull()) {
		kPrivate::for_axis(plot, level, [](QCPAxis* axis) {
			axis->grid()->setVisible(false); });
		return;
	}

	if (!jval.isObject())
		return;

	// ������Ϊ�ɼ�
	kPrivate::for_axis(plot, level, [](QCPAxis* axis) {
		axis->grid()->setVisible(true);
		});

	auto jobj = jval.toObject();

	/// ���ȴ�����������

	// ȫ�ֻ������ԣ������������÷�ʽ��һ��ֱ�����ã�����ͨ��line��������
	applyGridLine_(jval, plot, level); 
	if (jobj.contains("line"))
		applyGridLine_(jobj["line"], plot, level);

	if (jobj.contains("major"))
		applyGridMajor_(jobj["major"], plot, level);

	if (jobj.contains("minor"))
		applyGridMinor_(jobj["minor"], plot, level);

	if (jobj.contains("zeroline"))
		applyGridZeroline_(jobj["zeroline"], plot, level);


	/// Ȼ�����ػ�����
	kPrivate::handle_special_axes(jobj, plot, level, applyGrid_);
}


void KgPlotTheme::applyGridLine_(const QJsonValue& jval, QCustomPlot* plot, int level)
{
	kPrivate::for_axis(plot, level, [&jval](QCPAxis* axis) {
		if (jval.isNull()) {
			auto pen = axis->grid()->pen();
			pen.setStyle(Qt::NoPen);
			axis->grid()->setPen(pen);

			pen = axis->grid()->subGridPen();
			pen.setStyle(Qt::NoPen);
			axis->grid()->setSubGridPen(pen);

			pen = axis->grid()->zeroLinePen();
			pen.setStyle(Qt::NoPen);
			axis->grid()->setZeroLinePen(pen);
		}
		else if (jval.isObject()) {

			auto jobj = jval.toObject();

			auto pen = axis->grid()->pen();
			if (pen.style() == Qt::NoPen)
				pen.setStyle(Qt::DashLine);
			KuThemeUtil::apply(jobj, pen);
			axis->grid()->setPen(pen);

			pen = axis->grid()->subGridPen();
			if (pen.style() == Qt::NoPen)
				pen.setStyle(Qt::DashLine);
			KuThemeUtil::apply(jobj, pen);
			axis->grid()->setSubGridPen(pen);

			pen = axis->grid()->zeroLinePen();
			if (pen.style() == Qt::NoPen)
				pen.setStyle(Qt::DashLine);
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
			auto pen = axis->grid()->zeroLinePen();
			pen.setStyle(Qt::NoPen);
			axis->grid()->setZeroLinePen(pen); });
		return;
	}
	else if (!jval.isObject())
		return;

	auto jobj = jval.toObject();

	kPrivate::for_axis(plot, level, [&jobj](QCPAxis* axis) {
		QPen pen = axis->grid()->zeroLinePen();
		if (pen.style() == Qt::NoPen)
			pen.setStyle(Qt::SolidLine);
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