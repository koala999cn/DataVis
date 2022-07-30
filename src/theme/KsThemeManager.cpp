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


void KsThemeManager::applyCanvasBkgnd_(const QJsonValue& jval, KvThemedPlot* plot)
{
	auto bkgnd = plot->background();
	KuThemeParser::fill_value(jval, bkgnd, true);
	plot->setBackground(bkgnd);
}


void KsThemeManager::applyCanvasAxisRect_(const QJsonValue& jval, KvThemedPlot* plot)
{
	auto bkgnd = plot->axisBackground();
	KuThemeParser::fill_value(jval, bkgnd, true);
	plot->setAxisBackground(bkgnd);
}


void KsThemeManager::applyCanvasText_(const QJsonValue& jval, KvThemedPlot* plot)
{
	plot->applyTextColor(KvThemedPlot::k_text_all, [&jval](const QColor& color) {
		QColor newColor(color);
		KuThemeParser::color_value(jval, newColor);
		return newColor;
		});
}


void KsThemeManager::applyCanvasLine_(const QJsonValue& jval, KvThemedPlot* plot)
{
	plot->applyLine(KvThemedPlot::k_line_all, [&jval](const QPen& pen) {
		QPen newPen(pen);
		KuThemeParser::line_value(jval, newPen, false);
		return newPen;
		});
}


void KsThemeManager::applyCanvasGridline_(const QJsonValue& jval, KvThemedPlot* plot)
{
	plot->applyLine(KvThemedPlot::k_grid_line_all, [&jval](const QPen& pen) {
		QPen newPen(pen);
		KuThemeParser::line_value(jval, newPen, false);
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
		KuThemeParser::fill_value(jobj["background"], brush, true);
		plot->setBackground(brush);
	}
}


void KsThemeManager::tryLevel_(const QJsonObject& jobj, KvThemedPlot* plot, int level)
{
	if (jobj.contains("line")) 
		applyLine_(level & KvThemedPlot::k_line_all, jobj["line"], plot);


	if (jobj.contains("title")) {
		applyText_(level & KvThemedPlot::k_title_all, jobj["title"], plot);
		applyTextColor_(level & KvThemedPlot::k_title_all, jobj["title"], plot);
	}

	if (jobj.contains("label")) {
		applyText_(level & KvThemedPlot::k_label_all, jobj["label"], plot, true); // TODO: doShow不应该始终为true
		applyTextColor_(level & KvThemedPlot::k_label_all, jobj["label"], plot);
	}

	if (jobj.contains("text")) {
		applyText_(level & KvThemedPlot::k_text_all, jobj["text"], plot);
		applyTextColor_(level & KvThemedPlot::k_text_all, jobj["text"], plot);
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
			KuThemeParser::fill_value(jobj["background"], brush, true);
			plot->setAxisBackground(brush);
		}
	}

	// 解析每个坐标轴的属性
	applyAxis_(axis, plot, KvThemedPlot::k_axis_all);
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
		applyLine_(level & KvThemedPlot::k_axis_baseline_all, jobj["baseline"], plot);

	if (jobj.contains("tick")) 
		applyTick_(jobj["tick"], plot, level & KvThemedPlot::k_axis_tick_all);

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
		applyTick_(jobj["major"], plot, level & KvThemedPlot::k_axis_tick_major_all);


	if (jobj.contains("minor"))
		applyTick_(jobj["minor"], plot, level & KvThemedPlot::k_axis_tick_minor_all);


	/// 处理特化属性
	trySpecial_(jobj, plot, level, applyTick_);
}


void KsThemeManager::tryGrid_(const QJsonObject& jobj, KvThemedPlot* plot)
{
	if (!jobj.contains("grid")) return;

	auto grid = jobj["grid"];

	applyGrid_(grid, plot, KvThemedPlot::k_grid_all);
}


void KsThemeManager::applyGrid_(const QJsonValue& jval, KvThemedPlot* plot, int level)
{
	applyLine_(level, jval, plot);

	if (!jval.isObject())
		return; 

	auto jobj = jval.toObject();

	/// 首先处理公用属性
	tryLevel_(jobj, plot, level);

	if (jobj.contains("major")) {
		applyLine_(level & KvThemedPlot::k_grid_major_all, jobj["major"], plot, true);
	}

	if (jobj.contains("minor")) 
		applyLine_(level & KvThemedPlot::k_grid_minor_all, jobj["minor"], plot, true);

	if (jobj.contains("zeroline")) {
		applyLine_(level & KvThemedPlot::k_grid_zeroline_all, jobj["zeroline"], plot, true);
	}


	/// 然后处理特化属性
	trySpecial_(jobj, plot, level, applyGrid_);
}


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
	if (jobj.contains("x") && (level & KvThemedPlot::k_x)) // TODO: 这个判断算法有问题
		op(jobj["x"], plot, level & KvThemedPlot::k_x);

	if (jobj.contains("y") && (level & KvThemedPlot::k_y))
		op(jobj["y"], plot, level & KvThemedPlot::k_y);

	if (jobj.contains("top") && (level & KvThemedPlot::k_top))
		op(jobj["top"], plot, level & KvThemedPlot::k_top);

	if (jobj.contains("bottom") && (level & KvThemedPlot::k_bottom))
		op(jobj["bottom"], plot, level & KvThemedPlot::k_bottom);

	if (jobj.contains("left") && (level & KvThemedPlot::k_left))
		op(jobj["left"], plot, level & KvThemedPlot::k_left);

	if (jobj.contains("right") && (level & KvThemedPlot::k_right))
		op(jobj["right"], plot, level & KvThemedPlot::k_right);
}


void KsThemeManager::applyLine_(int level, const QJsonValue& jval, KvThemedPlot* plot, bool doShow)
{
	if (KuThemeParser::isNull(jval)) {
		plot->setVisible(level, false);
	}
	else {
		if (doShow) plot->setVisible(level, true);
		plot->applyLine(level, [&jval, level](const QPen& pen) {
			QPen newPen(pen);
			KuThemeParser::line_value(jval, newPen, !(level & KvThemedPlot::k_plot));
			return newPen;
			});
	}
}


void KsThemeManager::applyText_(int level, const QJsonValue& jval, KvThemedPlot* plot, bool doShow)
{
	if (KuThemeParser::isNull(jval)) {
		plot->setVisible(level, false);
	}
	else {
		if (doShow) plot->setVisible(level, true);

		plot->applyText(level, [&jval](const QFont& font) {
			QFont newFont(font);
			KuThemeParser::text_value(jval, newFont);
			return newFont;
			});
	}
}


void KsThemeManager::applyTextColor_(int level, const QJsonValue& jval, KvThemedPlot* plot)
{
	plot->applyTextColor(level, [&jval](const QColor& color) {
		QColor newColor(color);
		KuThemeParser::color_value(jval, newColor);
		return newColor;
		});
}

