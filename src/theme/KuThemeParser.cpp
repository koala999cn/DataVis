#include "KuThemeParser.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QBrush>
#include <QPen>
#include <QMargins>
#include <QFont>
#include <QColor>


bool KuThemeParser::isNull(const QJsonValue& jval)
{
	if (jval.isNull())
		return true;

	if (jval.isString()) {
		auto s = jval.toString();
		return s.compare("NA", Qt::CaseInsensitive) == 0
			|| s.compare("N/A", Qt::CaseInsensitive) == 0
			|| s.compare("N.A.", Qt::CaseInsensitive) == 0;
	}

	return false;
}


void KuThemeParser::fill_value(const QJsonValue& jval, QBrush& brush)
{
	if (isNull(jval)) {
		brush.setStyle(Qt::NoBrush);
	}
	else if (jval.isString()) {
		QColor color;
		if (color_value(jval.toString(), color)) {
			if (brush.style() == Qt::NoBrush)
				brush.setStyle(Qt::SolidPattern);

			brush.setColor(color);
		}
	}
	else if (jval.isObject()) {
		// TODO: 
		// 
	    // color

		// texture

		// gradient

		// style
	}
}


void KuThemeParser::line_value(const QJsonValue& jval, QPen& pen)
{
	if (isNull(jval)) {
		pen.setStyle(Qt::NoPen); 
		return;
	}
	
	if (jval.isString()) {
		auto str = jval.toString();
		if (!line_style(str, pen))  // 先尝试解析line-style
			// 失败则尝试解析line-color
			line_color(str, pen);
	}
	else if (jval.isDouble()) {
		auto w = jval.toDouble();
		pen.setWidthF(w);
	}
	else if (jval.isObject()) {
		auto jobj = jval.toObject();

		// color
		QColor color;
		if (tryColor(jobj, "color", color))
			pen.setColor(color);

		// width
		double width;
		if (tryDouble(jobj, "width", width))
			pen.setWidthF(width);

		// style
		QString style;
		if (tryString(jobj, "style", style))
			line_style(style, pen);
	}

	if (pen.style() == Qt::NoPen)
		pen.setStyle(Qt::SolidLine); // 置为可见. 由于无法得知隐藏前的线型，此处统一使用solid-line
}


void KuThemeParser::text_value(const QJsonValue& jval, QFont& font)
{
	if (isNull(jval) || jval.isString()) {
		; // do nothing
	}
	else if (jval.isDouble()) {
		auto w = jval.toDouble();
		font.setPointSizeF(w); // TODO: pixel size
	}
	else if (jval.isObject()) {
		auto jobj = jval.toObject();

		// family
		QString family;
		if (tryString(jobj, "family", family))
			font.setFamily(family);

		// face
		std::set<QString> face;
		if (tryStringEnum(jobj, "face", face)) {
			font.setBold(face.count("bold"));
			font.setItalic(face.count("italic"));
			font.setUnderline(face.count("underline"));
		}

		// size
		double size;
		if (tryDouble(jobj, "size", size))
			font.setPointSizeF(size); // TODO: pixel size
	}
}


void KuThemeParser::color_value(const QJsonValue& jval, QColor& color)
{
	if (isNull(jval))
		color = QColor("transparent");
	else if (jval.isString())
		color_value(jval.toString(), color);
	else if (jval.isObject())
		tryColor(jval.toObject(), "color", color);
}


std::vector<QColor> KuThemeParser::color_value_list(const QJsonArray& jar)
{
	std::vector<QColor> colors;
	colors.reserve(jar.size());
	for (auto i = jar.cbegin(); i != jar.cend(); ++i) {
		QColor color;
		if (i->isString() && color_value(i->toString(), color))
			colors.push_back(color);
	}

	return colors;
}


bool KuThemeParser::margins_value(const QJsonValue& jval, QMargins& margin)
{
	if (isNull(jval)) {
		margin = QMargins(0, 0, 0, 0);
	}
	else if (jval.isArray()) {
		auto ar = jval.toArray();
		std::vector<int> ints; ints.reserve(4);
		for (auto iter = ar.cbegin(); iter != ar.cend(); iter++) {
			if (iter->isDouble())
				ints.push_back(iter->toDouble());
		}

		if(ints.empty())
			return false;

		// int left, int top, int right, int bottom
		margin.setLeft(ints[0]);
		if (ints.size() > 1)
			margin.setTop(ints[1]);
		if (ints.size() > 2)
			margin.setRight(ints[2]);
		if (ints.size() > 3)
			margin.setBottom(ints[3]);
	}
	else if (jval.isObject()) {
		auto jobj = jval.toObject();
		int val;
		if (tryInt(jobj, "left", val))
			margin.setLeft(val);
		if (tryInt(jobj, "top", val))
			margin.setTop(val);
		if (tryInt(jobj, "right", val))
			margin.setRight(val);
		if (tryInt(jobj, "bottom", val))
			margin.setBottom(val);
	}
	else {
		return false;
	}

	return true; // TODO: 细化返回值
}


bool KuThemeParser::tryInt(const QJsonObject& jobj, const QString& name, int& val)
{
	if (jobj.contains(name) && jobj[name].isDouble()) {
		val = jobj[name].toInt();
		return true;
	}

	return false;
}


bool KuThemeParser::tryDouble(const QJsonObject& jobj, const QString& name, double& val)
{
	if (jobj.contains(name) && jobj[name].isDouble()) {
		val = jobj[name].toDouble();
		return true;
	}

	return false;
}


bool KuThemeParser::tryString(const QJsonObject& jobj, const QString& name, QString& val)
{
	if (jobj.contains(name) && jobj[name].isString()) {
		val = jobj[name].toString();
		return true;
	}

	return false;
}


bool KuThemeParser::tryStringEnum(const QJsonObject& jobj, const QString& name, std::set<QString>& val)
{
	if (jobj.contains(name)) {
		if (jobj[name].isString()) {
			val.insert(jobj[name].toString());
			return true;
		}
		else if (jobj[name].isArray()) {
			auto ar = jobj[name].toArray();
			for (auto iter = ar.begin(); iter != ar.end(); ++iter) {
				if (iter->isString())
					val.insert(iter->toString());
			}
			return true;
		}
	}

	return false;
}


bool KuThemeParser::tryBool(const QJsonObject& jobj, const QString& name, bool& val)
{
	if (jobj.contains(name) && jobj[name].isBool()) {
		val = jobj[name].toBool();
		return true;
	}

	return false;
}


bool KuThemeParser::tryColor(const QJsonObject& jobj, const QString& name, QColor& color)
{
	QString str;
	if (!tryString(jobj, name, str))
		return false;

	return color_value(str, color);
}


bool KuThemeParser::tryMargins(const QJsonObject& jobj, const QString& name, QMargins& margins)
{
	if (jobj.contains(name)) 
		return margins_value(jobj[name], margins);

	return false;
}


bool KuThemeParser::color_value(const QString& str, QColor& color)
{
	// 扩展支持rgb()格式, 如: rgb(198, 120, 65)
	if (0 == str.startsWith("rgb(", Qt::CaseInsensitive) && str.endsWith(')')) {
		auto s = str.mid(4);
		s.remove(s.size() - 1, 1); // remove tailing ')'
		auto rgb = s.split(',', Qt::SkipEmptyParts);
		if (rgb.size() != 3)
			return false;
		color.setRgb(rgb[0].toInt(), rgb[1].toInt(), rgb[2].toInt());
		return color.isValid();
	}

	// QColor支持的颜色字符格式：
	//  - #RGB(each of R, G, and B is a single hex digit)
	//	- #RRGGBB
	//	- #AARRGGBB(Since 5.2)
	//	- #RRRGGGBBB
	//	- #RRRRGGGGBBBB
	//	- transparent, representing the absence of a color
	//  - A list of SVG color keyword names provided by the World Wide Web Consortium.
	color.setNamedColor(str);
	return QColor::isValidColor(str);
}


bool KuThemeParser::line_style(const QString& str, QPen& pen)
{
	if (str == "solid")
		pen.setStyle(Qt::SolidLine);
	else if (str == "dash")
		pen.setStyle(Qt::DashLine);
	else if (str == "dot")
		pen.setStyle(Qt::DotLine);
	else if (str == "dashdot")
		pen.setStyle(Qt::DashDotLine);
	else if (str == "dashdotdot")
		pen.setStyle(Qt::DashDotDotLine);
	else
		return false;

	return true;
}


bool KuThemeParser::line_color(const QString& str, QPen& pen)
{
	QColor color;
	if (!color_value(str, color))
		return false;

	if (pen.style() == Qt::NoPen)
		pen.setStyle(Qt::SolidLine);

	pen.setColor(color);
	return true;

}

