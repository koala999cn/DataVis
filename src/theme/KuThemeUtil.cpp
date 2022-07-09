#include "KuThemeUtil.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QBrush>
#include <QPen>
#include <QMargins>
#include <QFont>
#include <QColor>


void KuThemeUtil::apply(const QJsonObject& jobj, QBrush& brush)
{
	// fill
	QColor color;
	if (tryColor(jobj, "fill", color))
		brush = color; // 不能使用setColor

	// texture

	// gradient

	// style

}


void KuThemeUtil::apply(const QJsonObject& jobj, QPen& pen)
{
	// color
	QColor color;
	if (tryColor(jobj, "color", color))
		pen.setColor(color);

	// width
	double width;
	if (tryDouble(jobj, "width", width))
		pen.setWidthF(width);

	// style


}


void KuThemeUtil::apply(const QJsonObject& jobj, QMargins& margin)
{
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


void KuThemeUtil::apply(const QJsonObject& jobj, QFont& font)
{
	// family
	QString family;
	if (tryString(jobj, "family", family))
		font.setFamily(family);

	// bold
	bool bold;
	if (tryBool(jobj, "bold", bold))
		font.setBold(bold);

	// italic
	bool italic;
	if (tryBool(jobj, "italic", italic))
		font.setItalic(italic);

	// size
	double size;
	if (tryDouble(jobj, "size", size))
		font.setPointSizeF(size); // TODO: pixel size

	// margin
}


void KuThemeUtil::apply(const QJsonObject& jobj, QGradient& grad)
{
	// TODO:
	assert(false);
}


bool KuThemeUtil::tryInt(const QJsonObject& jobj, const QString& name, int& val)
{
	if (jobj.contains(name) && jobj[name].isDouble()) {
		val = jobj[name].toInt();
		return true;
	}

	return false;
}


bool KuThemeUtil::tryDouble(const QJsonObject& jobj, const QString& name, double& val)
{
	if (jobj.contains(name) && jobj[name].isDouble()) {
		val = jobj[name].toDouble();
		return true;
	}

	return false;
}


bool KuThemeUtil::tryString(const QJsonObject& jobj, const QString& name, QString& val)
{
	if (jobj.contains(name) && jobj[name].isString()) {
		val = jobj[name].toString();
		return true;
	}

	return false;
}


bool KuThemeUtil::tryBool(const QJsonObject& jobj, const QString& name, bool& val)
{
	if (jobj.contains(name) && jobj[name].isBool()) {
		val = jobj[name].toBool();
		return true;
	}

	return false;
}


bool KuThemeUtil::tryColor(const QJsonObject& jobj, const QString& name, QColor& color)
{
	QString str;
	if (!tryString(jobj, name, str))
		return false;

	color.setNamedColor(str);
	return QColor::isValidColor(str);
}


bool KuThemeUtil::tryMargin(const QJsonObject& jobj, const QString& name, QMargins& margins)
{
	if (jobj.contains(name)) {
		if (jobj[name].isObject())
			apply(jobj[name].toObject(), margins);
		else if (jobj[name].isArray()) {
			auto ar = jobj[name].toArray();
			if (ar.size() != 4)
				return false;

			// int left, int top, int right, int bottom
			margins = { ar[0].toInt(), ar[1].toInt(), ar[2].toInt(), ar[3].toInt() };
		}

		return true;
	}

	return false;
}

