#pragma once
#include <map>
#include <string_view>
#include <vector>
#include "KtSingleton.h"
#include "KtColor.h"
#include "nlohmann/json.hpp"

class KvThemedPlot;


// ����json������KvThemedPlot�ӿ�

class KsThemeManager
{
public:
	using jobject = nlohmann::json;
	using jvalue = jobject;
	using stringlist = std::vector<std::string>;
	using colorlist = std::vector<color4f>;
	using theme_type = std::map<std::string, jobject>[4];
	using singleton_type = KtSingleton<KsThemeManager, false, true>;
	friend singleton_type;

	static KsThemeManager& singleton() {
		return *singleton_type::instance();
	}

	enum
	{
		k_theme,
		k_layout,
		k_canvas,
		k_palette
	};

	// @path: theme�ļ�·����������������ʽ����"theme\*.json"����������Ŀ¼
	bool load(const std::string_view& path);

	// �����Ѽ��صĸ�������Ԫ���б�
	stringlist listGroups() const; // �����б�������ȫ�ַ���
	stringlist listThemes(const std::string& group) const; // ���ⷽ��
	stringlist listCanvas(const std::string& group) const; // �ײ�����
	stringlist listPalettes(const std::string& group) const; // ��ɫ����
	stringlist listLayouts(const std::string& group) const; // ���ַ���

	void applyTheme(const std::string& group, const std::string& name, KvThemedPlot* plot) const;
	void applyLayout(const std::string& group, const std::string& name, KvThemedPlot* plot) const;
	void applyCanvas(const std::string& group, const std::string& name, KvThemedPlot* plot) const;
	void applyPalette(const std::string& group, const std::string& name, KvThemedPlot* plot) const;

	// ��������group::theme���õ�canvas���֣����ַ�����ʾ������
	std::string canvasName(const std::string& group, const std::string& theme);

	// ��������theme���õ�layout���֣����ַ�����ʾ������
	std::string layoutName(const std::string& group, const std::string& theme);

	// ��������theme���õ�palette���֣����ַ�����ʾ������
	std::string paletteName(const std::string& group, const std::string& theme);

	// ��ȡcanvas��ɫ��
	colorlist getCanvas(const std::string& group, const std::string& name);

	// ��ȡpalette��ɫ��
	bool getPalette(const std::string& group, const std::string& name, colorlist& majors, colorlist& minors);

	bool empty() const;

	bool hasTheme() const;
	bool hasLayout() const;
	bool hasCanvas() const;
	bool hasPalette() const;

	bool isEmpty(int themeType) const;

	stringlist listNames(int themeType, const std::string& groupName) const;;

protected:

	void tryLoad_(const jobject& jobj);
	void tryLoad_(theme_type& theme, const jobject& jobj);
	void tryObjectOrArray_(const jvalue& jval, std::function<void(const jobject&)> fn);
	void tryList_(const jobject& jobj, std::function<void(const std::string& key, const jvalue&)> fn);

	void tryCanvas_(const std::string& group, const jobject& jobj, KvThemedPlot* plot) const;
	void tryPalette_(const std::string& group, const jobject& jobj, KvThemedPlot* plot) const;
	void tryLayout_(const std::string& group, const jobject& jobj, KvThemedPlot* plot) const;

	void applyLayout_(const std::string& group, const jobject& jobj, KvThemedPlot* plot, bool inTheme) const; // TODO: �Ƿ���ҪinTheme
	void applyCanvas_(const jvalue& jval, KvThemedPlot* plot) const;
	void applyPalette_(const jvalue& jval, KvThemedPlot* plot) const;

	// ����true����ʾlevel�ɼ�������level���ɼ�
	static bool tryVisible_(int level, const jvalue& jval, KvThemedPlot* plot);

	// ����ȫ�����ԣ�Ŀǰ����line��text��title��label
	static void tryLevel_(int level, const jobject& jobj, KvThemedPlot* plot);

	static void tryBkgnd_(int level, const jobject& jobj, KvThemedPlot* plot);

	static void tryAxis_(const jobject& jobj, KvThemedPlot* plot);
	static void applyAxis_(int level, const jvalue& jval, KvThemedPlot* plot);
	static void applyTick_(int level, const jvalue& jval, KvThemedPlot* plot);

	static void tryGrid_(const jobject& jobj, KvThemedPlot* plot);
	static void applyGrid_(int level, const jvalue& jval, KvThemedPlot* plot);

	static void tryMargins_(int level, const jobject& jobj, KvThemedPlot* plot);
	static void tryLegend_(const jobject& jobj, KvThemedPlot* plot);

	static void tryAxisSpecials_(int level, const jobject& jobj, KvThemedPlot* plot,
		std::function<void(int, const jvalue&, KvThemedPlot*)> op);

	static void tryGridSpecials_(int level, const jobject& jobj, KvThemedPlot* plot,
		std::function<void(int, const jvalue&, KvThemedPlot*)> op);

	// ���jobj�Ƿ��������propName�������������enterLevel(level, propId)������һlevel�����ص�op
	// @newLevel: enterLevel(level, propId)�ķ��ؽ��
	// @jval: jobj[propName]
	static void trySpecialProp_(int level, const jobject& jobj, const char* propName, int propId,
		std::function<void(int newLevel, const jvalue& jval)> op);

	static void applyFill_(int level, const jvalue& jval, KvThemedPlot* plot);
	static void applyLine_(int level, const jvalue& jval, KvThemedPlot* plot);
	static void applyText_(int level, const jvalue& jval, KvThemedPlot* plot);
	static void applyTextColor_(int level, const jvalue& jval, KvThemedPlot* plot);

	static colorlist parseCanvas_(const jvalue& jval);
	static void parsePalette_(const jvalue& jval, colorlist& majors, colorlist& minors);

private:

	void removeInvalidThemes_();

	static bool isEmpty_(const theme_type& theme);

	const jobject& themeAt_(int type, const std::string& group, const std::string& name) const;

private:
	
	theme_type global_; // ȫ������(δ����)
	std::map<std::string, theme_type> grouped_; // ��������

private:
	KsThemeManager() = default;
	~KsThemeManager() = default;

	KsThemeManager(const KsThemeManager & app) = delete;
	void operator=(const KsThemeManager & app) = delete;
};
