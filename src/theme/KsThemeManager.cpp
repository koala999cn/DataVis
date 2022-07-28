#include "KsThemeManager.h"
#include "KuPathUtil.h"
#include "KuThemeParser.h"
#include <QFile>
#include <QBrush>
#include <QFont>
#include <QPen>
#include <QColor>
#include <QJsonDocument>
#include <QJsonArray>
#include <assert.h>
#include "KvThemedPlot.h"


bool KsThemeManager::load(const char* path)
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
				if (i->isObject()) // 不考虑递归数组的结构
					tryLoad_(i->toObject());
		}
	}

	removeInvalidThemes_();

	return !themes_.empty();
}


void KsThemeManager::tryLoad_(const QJsonObject& jobj)
{
	if (jobj.contains("theme"))
		tryObjectOrArray_(jobj["theme"], [this](const QJsonObject& jobj) {
			if (jobj.contains("name") && jobj["name"].isString()) // 忽略未命名的主题
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


void KsThemeManager::tryObjectOrArray_(const QJsonValue& jval, std::function<void(const QJsonObject&)> fn)
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


void KsThemeManager::tryList_(const QJsonObject& jobj, std::function<void(const QString& key, const QJsonValue&)> fn)
{
	for (auto i = jobj.begin(); i != jobj.end(); ++i)
	    fn(i.key(), *i);
}


void KsThemeManager::removeInvalidThemes_()
{
	// TODO:
}


QStringList KsThemeManager::listThemes() const
{
	QStringList list;
	for (auto i : themes_)
		list << i.first;

	return list;
}


QStringList KsThemeManager::listCanvas() const
{
	QStringList list;
	for (auto i : canvas_)
		list << i.first;

	return list;
}


QStringList KsThemeManager::listPalettes() const
{
	QStringList list;
	for (auto i : palettes_)
		list << i.first;

	return list;
}


QStringList KsThemeManager::listLayouts() const
{
	QStringList list;
	for (auto i : layouts_)
		list << i.first;

	return list;
}


QString KsThemeManager::canvasName(const QString& theme)
{
	if (themes_.count(theme) > 0) {
		auto jobj = themes_[theme];
		if (jobj.contains("canvas") && jobj["canvas"].isString())
			return jobj["canvas"].toString();
	}

	return "";
}


QString KsThemeManager::layoutName(const QString& theme)
{
	if (themes_.count(theme) > 0) {
		auto jobj = themes_[theme];
		if (jobj.contains("layout") && jobj["layout"].isString())
			return jobj["layout"].toString();
	}

	return "";
}


QString KsThemeManager::paletteName(const QString& theme)
{
	if (themes_.count(theme) > 0) {
		auto jobj = themes_[theme];
		if (jobj.contains("palette") && jobj["palette"].isString())
			return jobj["palette"].toString();
	}

	return "";
}


void KsThemeManager::applyTheme(const QString& name, KvThemedPlot* plot) const
{
	if (!themes_.count(name))
		return;

	auto& jobj = themes_.at(name);

	if (jobj.contains("base") && jobj["base"].isString())
		applyTheme(jobj["base"].toString(), plot);

	tryCanvas_(jobj, plot);
	tryPalette_(jobj, plot);
	tryLayout_(jobj, plot);
	applyLayout_(jobj, plot, true); // open 'inTheme' flag to disable "base" property
}


void KsThemeManager::applyLayout(const QString& name, KvThemedPlot* plot) const
{
	if (!layouts_.count(name))
		return;

	applyLayout_(layouts_.at(name), plot, false);
}


void KsThemeManager::applyCanvas(const QString& name, KvThemedPlot* plot) const
{
	if (!canvas_.count(name))
		return;

	applyCanvas_(canvas_.at(name), plot);
}


void KsThemeManager::applyPalette(const QString& name, KvThemedPlot* plot) const
{
	if (!palettes_.count(name))
		return;

	applyPalette_(palettes_.at(name), plot);
}


void KsThemeManager::tryCanvas_(const QJsonObject& jobj, KvThemedPlot* plot) const
{
	if (jobj.contains("canvas")) {
		auto canvas = jobj["canvas"];
		if (canvas.isString())
			applyCanvas(canvas.toString(), plot);
		else 
			applyCanvas_(canvas, plot);
	}
}


void KsThemeManager::tryPalette_(const QJsonObject& jobj, KvThemedPlot* plot) const
{
	if (jobj.contains("palette")) {
		auto palette = jobj["palette"];
		if (palette.isString())
			applyPalette(palette.toString(), plot);
		else 
			applyPalette_(palette, plot);
	}
}


void KsThemeManager::tryLayout_(const QJsonObject& jobj, KvThemedPlot* plot) const
{
	if (jobj.contains("layout")) {
		auto layout = jobj["layout"];
		if (layout.isString())
			applyLayout(layout.toString(), plot);
		else if (layout.isObject())
			applyLayout_(layout.toObject(), plot, false);
	}
}


void KsThemeManager::applyLayout_(const QJsonObject& jobj, KvThemedPlot* plot, bool inTheme) const
{
	if (!inTheme) { // try base
		if (jobj.contains("base") && jobj["base"].isString())
			applyLayout(jobj["base"].toString(), plot);
	}

	tryLevel_(jobj, plot, KvThemedPlot::k_all);
	tryBkgnd_(jobj, plot);
	tryAxis_(jobj, plot);
	tryGrid_(jobj, plot);
	tryMargins_(jobj, plot);
	tryLegend_(jobj, plot);
	trySpacing_(jobj, plot);
}


void KsThemeManager::applyCanvas_(const QJsonValue& jval, KvThemedPlot* plot) const
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

#if 0
namespace kPrivate
{
	void for_gridline(KvThemedPlot* plot, std::function<QPen(const QPen&)> op)
	{
		for (auto rect : plot->axisRects())
			for (auto axis : rect->axes()) {
				auto grid = axis->grid();
				grid->setPen(op(grid->pen()));
				grid->setSubGridPen(op(grid->subGridPen()));
				grid->setZeroLinePen(op(grid->zeroLinePen()));
			}
	}


	void for_line(KvThemedPlot* plot, std::function<QPen(const QPen&)> op)
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

		// TODO: data-line的theme谁控制？
	}
}
#endif

void KsThemeManager::applyCanvasBkgnd_(const QJsonValue& jval, KvThemedPlot* plot)
{
	auto bkgnd = plot->background();
	KuThemeParser::fill_value(jval, bkgnd);
	plot->setBackground(bkgnd);
}


void KsThemeManager::applyCanvasAxisRect_(const QJsonValue& jval, KvThemedPlot* plot)
{
	auto bkgnd = plot->axisBackground();
	KuThemeParser::fill_value(jval, bkgnd);
	plot->setAxisBackground(bkgnd);
}


void KsThemeManager::applyCanvasText_(const QJsonValue& jval, KvThemedPlot* plot)
{
	plot->applyTextColor(KvThemedPlot::k_all_text, [&jval](const QColor& color) { // TODO: level
		QColor newColor(color);
		KuThemeParser::color_value(jval, newColor);
		return newColor;
		});
}


void KsThemeManager::applyCanvasLine_(const QJsonValue& jval, KvThemedPlot* plot)
{
	plot->applyLine(KvThemedPlot::k_all_line, [&jval](const QPen& pen) {
		QPen newPen(pen);
		KuThemeParser::line_value(jval, newPen);
		return newPen;
		});
}


void KsThemeManager::applyCanvasGridline_(const QJsonValue& jval, KvThemedPlot* plot)
{
	plot->applyLine(KvThemedPlot::k_grid_line, [&jval](const QPen& pen) {
		QPen newPen(pen);
		KuThemeParser::line_value(jval, newPen);
		return newPen;
		});
}


void KsThemeManager::applyPalette_(const QJsonValue& jval, KvThemedPlot* plot) const
{
	std::vector<QColor> majors;
	std::vector<QColor> minors; // 用于outlining

	if (jval.isArray()) {
		majors = KuThemeParser::color_value_list(jval.toArray());
	}
	else if (jval.isObject()) {
		auto jobj = jval.toObject();
		if(jobj.contains("major") && jobj["major"].isArray())
			majors = KuThemeParser::color_value_list(jobj["major"].toArray());
		if (jobj.contains("minor")) {
			if (jobj["minor"].isArray())
				minors = KuThemeParser::color_value_list(jobj["minor"].toArray());
			else if (jobj["minor"].isString()) {
				QColor color;
				if (KuThemeParser::color_value(jobj["minor"].toString(), color))
					minors.push_back(color);
			}
		}
	}

	if (majors.empty())
		return;

	if (minors.empty())
		minors.push_back(QColor("NA"));

	for (unsigned i = 0; i < plot->numPlots(); i++) {
		// TODO: 插值，连续色等
		plot->applyPalette(i, majors[i % majors.size()], minors[0]); 
	}
}


void KsThemeManager::tryBkgnd_(const QJsonObject& jobj, KvThemedPlot* plot)
{
	if (jobj.contains("background")) {
		QBrush brush = plot->background();
		KuThemeParser::fill_value(jobj["background"], brush);
		plot->setBackground(brush);
	}
}


void KsThemeManager::tryLevel_(const QJsonObject& jobj, KvThemedPlot* plot, int level)
{
	if (jobj.contains("line")) 
		applyLine_(level & KvThemedPlot::k_all_line, jobj["line"], plot);


	if (jobj.contains("title")) {
		applyText_(level & KvThemedPlot::k_all_title, jobj["title"], plot);
		applyTextColor_(level & KvThemedPlot::k_all_title, jobj["title"], plot);
	}

	if (jobj.contains("label")) {
		applyText_(level & KvThemedPlot::k_all_label, jobj["label"], plot);
		applyTextColor_(level & KvThemedPlot::k_all_label, jobj["label"], plot);
	}

	if (jobj.contains("text")) {
		applyText_(level & KvThemedPlot::k_all_text, jobj["text"], plot);
		applyTextColor_(level & KvThemedPlot::k_all_text, jobj["text"], plot);
	}
}


void KsThemeManager::tryAxis_(const QJsonObject& jobj, KvThemedPlot* plot)
{
	if (!jobj.contains("axis")) return;
	
	auto axis = jobj["axis"];

	// 单独解析axis对象的背景属性，axis的其他元素没有该属性
	if (axis.isObject()) {
		auto jobj = axis.toObject();
		if (jobj.contains("background")) {
			QBrush brush = plot->axisBackground();
			KuThemeParser::fill_value(jobj["background"], brush);
			plot->setAxisBackground(brush);
		}
	}

	// 解析每个坐标轴的属性
	applyAxis_(axis, plot, KvThemedPlot::k_axis);

	// 若整个axis不可见，打开left & bottom坐标轴
	if (!plot->lineVisible(KvThemedPlot::k_axis)) 
		plot->setLineVisible(KvThemedPlot::k_axis_left | KvThemedPlot::k_axis_bottom, true);
}


void KsThemeManager::applyAxis_(const QJsonValue& jval, KvThemedPlot* plot, int level)
{
	applyLine_(level, jval, plot);

	if (!jval.isObject())
		return;

	auto jobj = jval.toObject();

	/// 首先处理公用属性
	tryLevel_(jobj, plot, level);

	if (jobj.contains("baseline"))
		applyLine_(level & KvThemedPlot::k_axis_baseline, jobj["baseline"], plot);

	if (jobj.contains("tick")) {
		applyTick_(jobj["tick"], plot, level & KvThemedPlot::k_axis_tick);
		//if (!plot->lineVisible(level))
		//	plot->setLineVisible(level & KvThemedPlot::k_axis_tick_major, true); // 默认主刻度可见
	}

	/// 然后处理特化属性
	trySpecial_(jobj, plot, level, applyAxis_);
}


void KsThemeManager::applyTick_(const QJsonValue& jval, KvThemedPlot* plot, int level)
{
	applyLine_(level, jval, plot);

	if (!jval.isObject())
		return;

	auto jobj = jval.toObject();

	QString str;
	KvThemedPlot::KeTickSide side = KvThemedPlot::k_tick_none;
	if (KuThemeParser::tryString(jobj, "side", str)) {
		if (str == "in")
			side = KvThemedPlot::k_tick_inside;
		else if (str == "out")
			side = KvThemedPlot::k_tick_outside;
		else if (str == "both")
			side = KvThemedPlot::k_tick_bothside;
	}

	int len(-1);
	if(KuThemeParser::tryInt(jobj, "length", len))
		plot->setTickLength(level, side, len);


	if (jobj.contains("major"))
		applyTick_(jobj["major"], plot, level & KvThemedPlot::k_axis_tick_major);


	if (jobj.contains("minor"))
		applyTick_(jobj["minor"], plot, level & KvThemedPlot::k_axis_tick_minor);


	/// 处理特化属性
	trySpecial_(jobj, plot, level, applyTick_);
}

/*
void KsThemeManager::applyAxesBaseline_(const QJsonValue& jval, KvThemedPlot* plot, int level)
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
			KuThemeParser::apply(jobj, pen);
			axis->setBasePen(pen); 
			});

		kPrivate::handle_special_axes(jobj, plot, level, applyAxesBaseline_);
	}
}
*/

/*
void KsThemeManager::applyAxesTitle_(const QJsonValue& jval, KvThemedPlot* plot, int level)
{
	// 可见性设置
	if(jval.isNull())
	    kPrivate::for_axis(plot, level, [&jval](QCPAxis* axis) {
		    // TODO: QCP没有setLabelVisible函数，只能通过设置透明色代替
		    axis->setLabelColor(QColor(Qt::transparent));
		    });
	else if (!jval.isObject())
		return;

	auto jobj = jval.toObject();

	QColor color;
	if (KuThemeParser::tryColor(jobj, "color", color))
		kPrivate::for_axis(plot, level, [color](QCPAxis* axis) {
		    axis->setLabelColor(color); });

	
	kPrivate::for_axis(plot, level, [&jobj](QCPAxis* axis) {
		QFont font = axis->labelFont();
		KuThemeParser::apply(jobj, font);
		    axis->setLabelFont(font); 
		});

	kPrivate::handle_special_axes(jobj, plot, level, applyAxesTitle_);
}


void KsThemeManager::applyAxesLabel_(const QJsonValue& jval, KvThemedPlot* plot, int level)
{
	// 可见性设置
	kPrivate::for_axis(plot, level, [&jval](QCPAxis* axis) {
		axis->setTickLabels(!jval.isNull());
		});

     if (!jval.isObject())
		return;

	auto jobj = jval.toObject();

	QColor color;
	if (KuThemeParser::tryColor(jobj, "color", color))
		kPrivate::for_axis(plot, level, [color](QCPAxis* axis) {
		axis->setTickLabelColor(color); });


	kPrivate::for_axis(plot, level, [&jobj](QCPAxis* axis) {
		QFont font = axis->labelFont();
		KuThemeParser::apply(jobj, font);
		axis->setTickLabelFont(font);
		});

	kPrivate::handle_special_axes(jobj, plot, level, applyAxesLabel_);

	if (jobj.contains("label") && jobj["label"].isObject()) {
		auto label = jobj["label"].toObject();
		QColor color;
		if (KuThemeParser::tryColor(label, "color", color))
			kPrivate::for_axis(plot, level, [color](QCPAxis* axis) {
			axis->setTickLabelColor(color); });

		kPrivate::for_axis(plot, level, [&label](QCPAxis* axis) {
			auto font = axis->labelFont();
			KuThemeParser::apply(label, font);
			axis->setTickLabelFont(font);
			});
	}
}*/


void KsThemeManager::tryGrid_(const QJsonObject& jobj, KvThemedPlot* plot)
{
	if (!jobj.contains("grid")) return;

	auto grid = jobj["grid"];

	applyGrid_(grid, plot, KvThemedPlot::k_grid);

	// 若整个grid不可见，打开major栅格
	if (!plot->lineVisible(KvThemedPlot::k_grid))
		plot->setLineVisible(KvThemedPlot::k_grid_major & KvThemedPlot::k_grid, true);
}


void KsThemeManager::applyGrid_(const QJsonValue& jval, KvThemedPlot* plot, int level)
{
	applyLine_(level, jval, plot);

	if (!jval.isObject())
		return; 

	auto jobj = jval.toObject();

	/// 首先处理公用属性
	tryLevel_(jobj, plot, level);

	if (jobj.contains("major"))
		applyLine_(level & KvThemedPlot::k_grid_major, jobj["major"], plot);

	if (jobj.contains("minor")) 
		applyLine_(level & KvThemedPlot::k_grid_minor, jobj["minor"], plot);

	if (jobj.contains("zeroline"))
		applyLine_(level & KvThemedPlot::k_grid_zeroline, jobj["zeroline"], plot);


	/// 然后处理特化属性
	trySpecial_(jobj, plot, level, applyGrid_);
}

/*
void KsThemeManager::applyGridLine_(const QJsonValue& jval, KvThemedPlot* plot, int level)
{
	kPrivate::for_axis(plot, level, [&jval](QCPAxis* axis) {

		axis->grid()->setVisible(!jval.isNull());
		axis->grid()->setSubGridVisible(!jval.isNull());

		if (jval.isObject()) {

			auto jobj = jval.toObject();

			auto pen = axis->grid()->pen();
			assert(pen.style() != Qt::NoPen);
			KuThemeParser::apply(jobj, pen);
			axis->grid()->setPen(pen);

			pen = axis->grid()->subGridPen();
			assert(pen.style() != Qt::NoPen);
			KuThemeParser::apply(jobj, pen);
			axis->grid()->setSubGridPen(pen);

			pen = axis->grid()->zeroLinePen();
			if (pen.style() == Qt::NoPen)
				pen.setStyle(Qt::SolidLine);
			KuThemeParser::apply(jobj, pen);
			axis->grid()->setZeroLinePen(pen);
		}
		});
}


void KsThemeManager::applyGridMajor_(const QJsonValue& jval, KvThemedPlot* plot, int level)
{
	kPrivate::for_axis(plot, level, [&jval](QCPAxis* axis) {
		axis->grid()->setVisible(!jval.isNull()); });

	 if (!jval.isObject())
		return;

	auto jobj = jval.toObject();

	kPrivate::for_axis(plot, level, [&jobj](QCPAxis* axis) {
		QPen pen = axis->grid()->pen();
		KuThemeParser::apply(jobj, pen);
		axis->grid()->setPen(pen); 
		});

	kPrivate::handle_special_axes(jobj, plot, level, applyGridMajor_);
}


void KsThemeManager::applyGridMinor_(const QJsonValue& jval, KvThemedPlot* plot, int level)
{
	kPrivate::for_axis(plot, level, [&jval](QCPAxis* axis) {
		axis->grid()->setSubGridVisible(!jval.isNull()); });

	 if (!jval.isObject())
		return;

	auto jobj = jval.toObject();

	kPrivate::for_axis(plot, level, [&jobj](QCPAxis* axis) {
		QPen pen = axis->grid()->subGridPen();
		assert(pen.style() != Qt::NoPen);
		if(axis->grid()->visible())
		    assert(axis->grid()->subGridVisible());
		KuThemeParser::apply(jobj, pen);
		axis->grid()->setSubGridPen(pen);
		});

	kPrivate::handle_special_axes(jobj, plot, level, applyGridMinor_);
}


void KsThemeManager::applyGridZeroline_(const QJsonValue& jval, KvThemedPlot* plot, int level)
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
		KuThemeParser::apply(jobj, pen);
		axis->grid()->setZeroLinePen(pen);
		});

	kPrivate::handle_special_axes(jobj, plot, level, applyGridZeroline_);
}
*/

void KsThemeManager::tryMargins_(const QJsonObject& jobj, KvThemedPlot* plot)
{
	if (jobj.contains("margins")) {
		auto margins = plot->margins();
		if (KuThemeParser::margins_value(jobj["margins"], margins))
			plot->setMargins(margins);
	}
}


void KsThemeManager::tryLegend_(const QJsonObject& jobj, KvThemedPlot* plot)
{

}


void KsThemeManager::trySpacing_(const QJsonObject& jobj, KvThemedPlot* plot)
{

}

void KsThemeManager::trySpecial_(const QJsonObject& jobj, KvThemedPlot* plot, int level,
	std::function<void(const QJsonValue&, KvThemedPlot*, int)> op)
{
	if (jobj.contains("x") && (level & KvThemedPlot::k_axis_x))
		op(jobj["x"], plot, level & KvThemedPlot::k_axis_x);

	if (jobj.contains("y") && (level & KvThemedPlot::k_axis_y))
		op(jobj["y"], plot, level & KvThemedPlot::k_axis_y);

	if (jobj.contains("top") && (level & KvThemedPlot::k_axis_top))
		op(jobj["top"], plot, level & KvThemedPlot::k_axis_top);

	if (jobj.contains("bottom") && (level & KvThemedPlot::k_axis_bottom))
		op(jobj["bottom"], plot, level & KvThemedPlot::k_axis_bottom);

	if (jobj.contains("left") && (level & KvThemedPlot::k_axis_left))
		op(jobj["left"], plot, level & KvThemedPlot::k_axis_left);

	if (jobj.contains("right") && (level & KvThemedPlot::k_axis_right))
		op(jobj["right"], plot, level & KvThemedPlot::k_axis_right);
}


void KsThemeManager::applyLine_(int level, const QJsonValue& jval, KvThemedPlot* plot)
{
	plot->applyLine(level, [&jval](const QPen& pen) {
		QPen newPen(pen);
		KuThemeParser::line_value(jval, newPen);
		return newPen;
		});
}


void KsThemeManager::applyText_(int level, const QJsonValue& jval, KvThemedPlot* plot)
{
	plot->applyText(level, [&jval](const QFont& font) {
		QFont newFont(font);
		KuThemeParser::text_value(jval, newFont);
		return newFont;
		});
}


void KsThemeManager::applyTextColor_(int level, const QJsonValue& jval, KvThemedPlot* plot)
{
	plot->applyTextColor(level, [&jval](const QColor& color) {
		QColor newColor(color);
		KuThemeParser::color_value(jval, newColor);
		return newColor;
		});
}

