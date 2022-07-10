#pragma once
#include "base/KmNonCopyable.h"
#include <set>


class QString;
class QJsonObject;
class QBrush;
class QPen;
class QMargins;
class QFont;
class QGradient;
class QColor;


class KuThemeUtil : public KmNonCopyable
{
public:

	static void apply(const QJsonObject& jobj, QBrush& brush);

	static void apply(const QJsonObject& jobj, QPen& pen);

	static void apply(const QJsonObject& jobj, QMargins& margin);

	static void apply(const QJsonObject& jobj, QFont& font);

	static void apply(const QJsonObject& jobj, QGradient& grad);

	static bool tryInt(const QJsonObject& jobj, const QString& name, int& val);

	static bool tryDouble(const QJsonObject& jobj, const QString& name, double& val);

	static bool tryString(const QJsonObject& jobj, const QString& name, QString& val);

	static bool tryStringEnum(const QJsonObject& jobj, const QString& name, std::set<QString>& val);

	static bool tryBool(const QJsonObject& jobj, const QString& name, bool& val);

	static bool tryColor(const QJsonObject& jobj, const QString& name, QColor& color);

	static bool tryMargin(const QJsonObject& jobj, const QString& name, QMargins& margins);
};