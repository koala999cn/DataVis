#pragma once
#include <map>
#include <string>
#include <vector>
#include "KtSingleton.h"
#include "KtColor.h"
#include "nlohmann/json.hpp"

class KvThemedPlot;


// 链接json对象与KvThemedPlot接口

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

	// @path: theme文件路径，可以是正则表达式，如"theme\*.json"，但不能是目录
	bool load(const char* path);

	// 返回已加载的各类主题元素列表
	stringlist listGroups() const; // 主题类别
	stringlist listThemes() const; // 主题方案
	stringlist listCanvas() const; // 底布方案
	stringlist listPalettes() const; // 配色方案
	stringlist listLayouts() const; // 布局方案

	void applyTheme(const std::string& name, KvThemedPlot* plot) const;
	void applyLayout(const std::string& name, KvThemedPlot* plot) const;
	void applyCanvas(const std::string& name, KvThemedPlot* plot) const;
	void applyPalette(const std::string& name, KvThemedPlot* plot) const;

	// 返回主题theme引用的canvas名字，空字符串表示无引用
	std::string canvasName(const std::string& theme);

	// 返回主题theme引用的layout名字，空字符串表示无引用
	std::string layoutName(const std::string& theme);

	// 返回主题theme引用的palette名字，空字符串表示无引用
	std::string paletteName(const std::string& theme);

	// 获取canvas的色带
	colorlist getCanvas(const std::string& name);

	// 获取palette的色带
	bool getPalette(const std::string& name, colorlist& majors, colorlist& minors);

protected:

	void tryLoad_(const jobject& jobj);
	void tryObjectOrArray_(const jvalue& jval, std::function<void(const jobject&)> fn);
	void tryList_(const jobject& jobj, std::function<void(const std::string& key, const jvalue&)> fn);

	void tryCanvas_(const jobject& jobj, KvThemedPlot* plot) const;
	void tryPalette_(const jobject& jobj, KvThemedPlot* plot) const;
	void tryLayout_(const jobject& jobj, KvThemedPlot* plot) const;

	void applyLayout_(const jobject& jobj, KvThemedPlot* plot, bool inTheme) const; // TODO: 是否需要inTheme
	void applyCanvas_(const jvalue& jval, KvThemedPlot* plot) const;
	void applyPalette_(const jvalue& jval, KvThemedPlot* plot) const;

	static void tryVisible_(int level, const jvalue& jval, KvThemedPlot* plot);

	// 解析全局属性，目前包括line，text，title，label
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

	// 检测jobj是否包含属性propName，若包含则调用enterLevel(level, propId)进入下一level，并回调op
	// @newLevel: enterLevel(level, propId)的返回结果
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


	std::map<std::string, std::pair<int, std::string>> groups_; // int表示类型，取值k_theme, k_canvas, k_palette, k_layout
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
