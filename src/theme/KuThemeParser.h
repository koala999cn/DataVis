#pragma once
#include "base/KmNonCopyable.h"
#include <set>
#include <vector>
#include <QColor>


class QJsonObject;
class QJsonValue;
class QJsonArray;
class QString;
class QBrush;
class QPen;
class QMargins;
class QFont;
class QGradient;


// 实现与plot接口无关的theme文件解析函数
// 与词法分析有关的函数使用了全小写命名方式

class KuThemeParser : public KmNonCopyable
{
public:

	////////////////////////////////////////////////////////////////////////////////
	///
	/// 以下函数解析jobj对象的特定属性
	/// 避免重复调用jobj.contains(name) & jobj[name].isXxx() & jobj[name].toXxx()
	/// 

	static bool isNull(const QJsonValue& jval);

	static bool tryInt(const QJsonObject& jobj, const QString& name, int& val);

	static bool tryDouble(const QJsonObject& jobj, const QString& name, double& val);

	static bool tryString(const QJsonObject& jobj, const QString& name, QString& val);

	static bool tryStringEnum(const QJsonObject& jobj, const QString& name, std::set<QString>& val);

	static bool tryBool(const QJsonObject& jobj, const QString& name, bool& val);

	static bool tryColor(const QJsonObject& jobj, const QString& name, QColor& color);

	static bool tryMargins(const QJsonObject& jobj, const QString& name, QMargins& margins);


	////////////////////////////////////////////////////////////////////////////////
	///
	/// 以下函数解析绘图所需的GUI元素
	/// 

	// 解析颜色字符串，若成功则将结果应用到color
	static bool color_value(const QString& str, QColor& color);

	static bool line_style(const QString& str, QPen& pen);

	static bool line_color(const QString& str, QPen& pen);

	static void line_value(const QJsonValue& jval, QPen& pen);

	static void fill_value(const QJsonValue& jval, QBrush& brush);

	static void text_value(const QJsonValue& jval, QFont& font);

	static void color_value(const QJsonValue& jval, QColor& color);

	static std::vector<QColor> color_value_list(const QJsonArray& jar);

	static bool margins_value(const QJsonValue& jval, QMargins& margin);

};