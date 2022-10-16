#pragma once
#include <vector>
#include <set>
#include "KmNonCopyable.h"
#include "KtColor.h"
#include "KtMargins.h"
#include "KpContext.h"
#include "nlohmann/json.hpp"


// ʵ����plot�ӿ��޹ص�theme�ļ���������
// ��ʷ������йصĺ���ʹ����ȫСд������ʽ

class KuThemeParser : public KmNonCopyable
{
public:
	using jobject = nlohmann::json;
	using jvalue = jobject;

	////////////////////////////////////////////////////////////////////////////////
	///
	/// ���º�������jobj������ض�����
	/// �����ظ�����jobj.contains(name) & jobj[name].isXxx() & jobj[name].toXxx()
	/// 

	static bool isNull(const jvalue& jval);

	static bool tryInt(const jobject& jobj, const std::string& name, int& val);

	static bool tryDouble(const jobject& jobj, const std::string& name, double& val);

	static bool tryString(const jobject& jobj, const std::string& name, std::string& val);

	static bool tryStringEnum(const jobject& jobj, const std::string& name, std::set<std::string>& val);

	static bool tryBool(const jobject& jobj, const std::string& name, bool& val);

	static bool tryColor(const jobject& jobj, const std::string& name, color4f& color);

	static bool tryMargins(const jobject& jobj, const std::string& name, KtMargins<double>& margins);


	////////////////////////////////////////////////////////////////////////////////
	///
	/// ���º���������ͼ�����GUIԪ��
	/// 

	// ������ɫ�ַ��������ɹ��򽫽��Ӧ�õ�color
	static bool color_value(const std::string& str, color4f& color);

	static bool line_style(const std::string& str, KpPen& pen);

	static bool line_color(const std::string& str, KpPen& pen);

	static void line_value(const jvalue& jval, KpPen& pen);

	static void fill_value(const jvalue& jval, KpBrush& brush);

	static void text_value(const jvalue& jval, KpFont& font);

	static void color_value(const jvalue& jval, color4f& color);

	static std::vector<color4f> color_value_list(const jobject& jar);

	static bool margins_value(const jvalue& jval, KtMargins<double>& margin);

};
