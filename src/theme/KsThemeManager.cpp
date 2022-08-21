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

	tryLevel_(KvThemedPlot::k_all, jobj, plot);
	tryBkgnd_(KvThemedPlot::k_plot, jobj, plot);
	tryAxis_(jobj, plot);
	tryGrid_(jobj, plot);
	tryMargins_(KvThemedPlot::k_plot, jobj, plot);
	tryLegend_(jobj, plot);
}


void KsThemeManager::applyCanvas_(const QJsonValue& jval, KvThemedPlot* plot) const
{
	if (jval.isObject()) {
		auto jobj = jval.toObject();

		tryBkgnd_(KvThemedPlot::k_plot, jobj, plot);

		if (jobj.contains("axis-rect"))
			applyFill_(KvThemedPlot::k_axis, jobj["axis-rect"], plot);

		if (jobj.contains("text"))
			applyTextColor_(KvThemedPlot::k_text_all, jobj["text"], plot);

		if (jobj.contains("line"))
			applyLine_(KvThemedPlot::k_line_all, jobj["line"], plot);

		if (jobj.contains("gridline"))
			applyLine_(KvThemedPlot::k_grid_line_all, jobj["gridline"], plot); 
	}
	else if (jval.isArray()) {
		auto ar = jval.toArray();
		auto sz = ar.size();

		if (sz > 0)
			applyFill_(KvThemedPlot::k_plot, ar.at(0), plot);

		if (sz > 1)
			applyFill_(KvThemedPlot::k_axis, ar.at(1), plot);

		if (sz > 2)
			applyText_(KvThemedPlot::k_text_all, ar.at(2), plot);

		if (sz > 3)
			applyLine_(KvThemedPlot::k_line_all, ar.at(3), plot);

		if (sz > 4)
			applyLine_(KvThemedPlot::k_grid_line_all, ar.at(4), plot);
	}
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


void KsThemeManager::tryBkgnd_(int level, const QJsonObject& jobj, KvThemedPlot* plot)
{
	if (jobj.contains("background"))
		applyFill_(level, jobj["background"], plot);
}


void KsThemeManager::applyFill_(int level, const QJsonValue& jval, KvThemedPlot* plot)
{
	QBrush brush = plot->fill(level);
	KuThemeParser::fill_value(jval, brush);
	plot->applyFill(level, brush);
}


int KsThemeManager::tryVisible_(int level, const QJsonValue& jval, KvThemedPlot* plot)
{
	if (KuThemeParser::isNull(jval)) {
		plot->applyVisible(level, false);
		return -1;
	}
	else if (jval.isBool()) {
		bool b = jval.toBool();
		plot->applyVisible(level, b);
		return b ? 1 : -1;
	}
	else if (jval.isObject()) {
		bool b(true);
		if (KuThemeParser::tryBool(jval.toObject(), "visible", b)) {
			plot->applyVisible(level, b);
			return b ? 1 : -1;
		}
	}

	return 0;
}


void KsThemeManager::tryLevel_(int level, const QJsonObject& jobj, KvThemedPlot* plot)
{
	trySpecialProp_(level, jobj, "line", KvThemedPlot::k_line,
		[plot](int newLevel, const QJsonValue& jval) {
			applyLine_(newLevel, jval, plot);
		});

	trySpecialProp_(level, jobj, "title", KvThemedPlot::k_title,
		[plot](int newLevel, const QJsonValue& jval) {
			applyText_(newLevel, jval, plot);
			applyTextColor_(newLevel, jval, plot);
		});

	trySpecialProp_(level, jobj, "label", KvThemedPlot::k_label,
		[plot](int newLevel, const QJsonValue& jval) {
			applyText_(newLevel, jval, plot);
			applyTextColor_(newLevel, jval, plot);
		});

	trySpecialProp_(level, jobj, "text", KvThemedPlot::k_text,
		[plot](int newLevel, const QJsonValue& jval) {
			applyText_(newLevel, jval, plot);
			applyTextColor_(newLevel, jval, plot);
		});
}


void KsThemeManager::tryAxis_(const QJsonObject& jobj, KvThemedPlot* plot)
{
	if (!jobj.contains("axis")) return;
	
	auto axis = jobj["axis"];

	// 单独解析axis对象的背景属性，axis的其他元素没有该属性
	if (axis.isObject()) {
		auto jobj = axis.toObject();
		tryBkgnd_(KvThemedPlot::k_axis, jobj, plot);
		tryMargins_(KvThemedPlot::k_axis, jobj, plot);
	}

	// 解析每个方向(left, top, right, bottom)坐标轴的属性
	applyAxis_(KvThemedPlot::k_axis_all, axis, plot);
}


void KsThemeManager::applyAxis_(int level, const QJsonValue& jval, KvThemedPlot* plot)
{
	tryVisible_(level, jval, plot);

	if (!jval.isObject()) 
		return;

	applyLine_(level, jval, plot); // 放在isObject检测之后，主要为防止applyLine_再次进行tryVisible_检测

	auto jobj = jval.toObject();

	/// 首先处理公用属性
	tryLevel_(level, jobj, plot);

	trySpecialProp_(level, jobj, "baseline", KvThemedPlot::k_axis_baseline,
		[plot](int newLevel, const QJsonValue& jval) {
			applyLine_(newLevel, jval, plot);
		});

	trySpecialProp_(level, jobj, "tick", KvThemedPlot::k_axis_tick,
		[plot](int newLevel, const QJsonValue& jval) {
			applyTick_(newLevel, jval, plot);
		});


	/// 然后处理特化属性
	trySpecials_(level, jobj, plot, applyAxis_);
}


void KsThemeManager::applyTick_(int level, const QJsonValue& jval, KvThemedPlot* plot)
{
	tryVisible_(level, jval, plot);

	if (!jval.isObject())
		return;

	applyLine_(level, jval, plot); // 放在isObject检测之后，主要为防止applyLine_再次进行tryVisible_检测

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


	trySpecialProp_(level, jobj, "major", KvThemedPlot::k_axis_tick_major, 
		[plot](int newLevel, const QJsonValue& jval) {
			applyTick_(newLevel, jval, plot);
		});

	trySpecialProp_(level, jobj, "minor", KvThemedPlot::k_axis_tick_minor,
		[plot](int newLevel, const QJsonValue& jval) {
			applyTick_(newLevel, jval, plot);
		});

	/// 处理特化属性
	trySpecials_(level, jobj, plot, applyTick_);
}


void KsThemeManager::tryGrid_(const QJsonObject& jobj, KvThemedPlot* plot)
{
	if (!jobj.contains("grid")) return;

	auto grid = jobj["grid"];

	applyGrid_(KvThemedPlot::k_grid_all, grid, plot);
}


void KsThemeManager::applyGrid_(int level, const QJsonValue& jval, KvThemedPlot* plot)
{
	tryVisible_(level, jval, plot); 

	if (!jval.isObject())
		return; 

	applyLine_(level, jval, plot); // 放在isObject检测之后，主要为防止applyLine_再次进行tryVisible_检测

	auto jobj = jval.toObject();

	/// 1. 处理公用属性(line, text, label, ...)
	tryLevel_(level, jobj, plot);

	/// 2. 处理特有属性
	static const std::pair<const char*, int> prop[] = {
		{ "major", KvThemedPlot::k_grid_major },
		{ "minor", KvThemedPlot::k_grid_minor },
		{ "zeroline", KvThemedPlot::k_grid_zeroline }
	};
	for (unsigned i = 0; i < sizeof(prop) / sizeof(prop[0]); i++) 
		trySpecialProp_(level, jobj, prop[i].first, prop[i].second,
			[plot](int newLevel, const QJsonValue& jval) {
				applyLine_(newLevel, jval, plot);
			}
	    );


	/// 3. 处理特化属性(x, y, left, ...)
	trySpecials_(level, jobj, plot, applyGrid_);
}


// TODO: 按照level处理
void KsThemeManager::tryMargins_(int level, const QJsonObject& jobj, KvThemedPlot* plot)
{
	if (jobj.contains("margins")) {
		auto margins = plot->margins(level);
		if (KuThemeParser::margins_value(jobj["margins"], margins))
			plot->applyMargins(level, margins);
	}
}


void KsThemeManager::tryLegend_(const QJsonObject& jobj, KvThemedPlot* plot)
{

}


void KsThemeManager::trySpecials_(int level, const QJsonObject& jobj, KvThemedPlot* plot,
	std::function<void(int, const QJsonValue&, KvThemedPlot*)> op)
{
	static const std::pair<const char*, int> special[] = {
		{ "x", KvThemedPlot::k_x },
		{ "y", KvThemedPlot::k_y },
		{ "left", KvThemedPlot::k_left },
		{ "top", KvThemedPlot::k_top },
		{ "right", KvThemedPlot::k_right },
		{ "bottom", KvThemedPlot::k_bottom }
	};

	for (unsigned i = 0; i < sizeof(special) / sizeof(special[0]); i++)
		trySpecialProp_(level, jobj, special[i].first, special[i].second, 
			[plot, op](int newLevel, const QJsonValue& jval) {
				op(newLevel, jval, plot);
			}
		);
}


void KsThemeManager::trySpecialProp_(int level, const QJsonObject& jobj, const char* propName, int propId,
	std::function<void(int, const QJsonValue&)> op)
{
	auto newLevel = KvThemedPlot::enterLevel(level, propId);
	if (jobj.contains(propName) && newLevel)
		op(newLevel, jobj[propName]);
}


void KsThemeManager::applyLine_(int level, const QJsonValue& jval, KvThemedPlot* plot)
{
	tryVisible_(level, jval, plot);

	plot->applyLine(level, [&jval, level](const QPen& pen) {
		QPen newPen(pen);
		KuThemeParser::line_value(jval, newPen);
		return newPen;
		});
}


void KsThemeManager::applyText_(int level, const QJsonValue& jval, KvThemedPlot* plot)
{
	tryVisible_(level, jval, plot);

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
