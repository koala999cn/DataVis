#pragma once
#include <map>
#include <string>
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
	using singleton_type = KtSingleton<KsThemeManager, false, true>;
	friend singleton_type;

	static KsThemeManager& singleton() {
		return *singleton_type::instance();
	}

	// @path: theme�ļ�·����������������ʽ����"theme\*.json"����������Ŀ¼
	bool load(const char* path);

	// �����Ѽ��صĸ�������Ԫ���б�
	stringlist listGroups() const; // �������
	stringlist listThemes() const; // ���ⷽ��
	stringlist listCanvas() const; // �ײ�����
	stringlist listPalettes() const; // ��ɫ����
	stringlist listLayouts() const; // ���ַ���

	void applyTheme(const std::string& name, KvThemedPlot* plot) const;
	void applyLayout(const std::string& name, KvThemedPlot* plot) const;
	void applyCanvas(const std::string& name, KvThemedPlot* plot) const;
	void applyPalette(const std::string& name, KvThemedPlot* plot) const;

	// ��������theme���õ�canvas���֣����ַ�����ʾ������
	std::string canvasName(const std::string& theme);

	// ��������theme���õ�layout���֣����ַ�����ʾ������
	std::string layoutName(const std::string& theme);

	// ��������theme���õ�palette���֣����ַ�����ʾ������
	std::string paletteName(const std::string& theme);

	// ��ȡcanvas��ɫ��
	colorlist getCanvas(const std::string& name);

	// ��ȡpalette��ɫ��
	bool getPalette(const std::string& name, colorlist& majors, colorlist& minors);

protected:

	void tryLoad_(const jobject& jobj);
	void tryObjectOrArray_(const jvalue& jval, std::function<void(const jobject&)> fn);
	void tryList_(const jobject& jobj, std::function<void(const std::string& key, const jvalue&)> fn);

	void tryCanvas_(const jobject& jobj, KvThemedPlot* plot) const;
	void tryPalette_(const jobject& jobj, KvThemedPlot* plot) const;
	void tryLayout_(const jobject& jobj, KvThemedPlot* plot) const;

	void applyLayout_(const jobject& jobj, KvThemedPlot* plot, bool inTheme) const; // TODO: �Ƿ���ҪinTheme
	void applyCanvas_(const jvalue& jval, KvThemedPlot* plot) const;
	void applyPalette_(const jvalue& jval, KvThemedPlot* plot) const;

	static void tryVisible_(int level, const jvalue& jval, KvThemedPlot* plot);

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

private:
	enum
	{
		k_theme,
		k_canvas,
		k_palette,
		k_layout
	};


	std::map<std::string, std::pair<int, std::string>> groups_; // int��ʾ���ͣ�ȡֵk_theme, k_canvas, k_palette, k_layout
	std::map<std::string, jobject> themes_;
	std::map<std::string, jvalue> canvas_;
	std::map<std::string, jvalue> palettes_;
	std::map<std::string, jobject> layouts_;

private:
	KsThemeManager() = default;
	~KsThemeManager() = default;

	KsThemeManager(const KsThemeManager & app) = delete;
	void operator=(const KsThemeManager & app) = delete;
};
