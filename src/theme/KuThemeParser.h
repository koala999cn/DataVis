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


// ʵ����plot�ӿ��޹ص�theme�ļ���������
// ��ʷ������йصĺ���ʹ����ȫСд������ʽ

class KuThemeParser : public KmNonCopyable
{
public:

	////////////////////////////////////////////////////////////////////////////////
	///
	/// ���º�������jobj������ض�����
	/// �����ظ�����jobj.contains(name) & jobj[name].isXxx() & jobj[name].toXxx()
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
	/// ���º���������ͼ�����GUIԪ��
	/// 

	// ������ɫ�ַ��������ɹ��򽫽��Ӧ�õ�color
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