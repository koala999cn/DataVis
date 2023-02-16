#pragma once
#include <map>
#include <string_view>
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

	// @path: theme文件路径，可以是正则表达式，如"theme\*.json"，但不能是目录
	bool load(const std::string_view& path);

	// 返回已加载的各类主题元素列表
	stringlist listGroups() const; // 分组列表，不包含全局分组
	stringlist listThemes(const std::string& group) const; // 主题方案
	stringlist listCanvas(const std::string& group) const; // 底布方案
	stringlist listPalettes(const std::string& group) const; // 配色方案
	stringlist listLayouts(const std::string& group) const; // 布局方案

	void applyTheme(const std::string& group, const std::string& name, KvThemedPlot* plot) const;
	void applyLayout(const std::string& group, const std::string& name, KvThemedPlot* plot) const;
	void applyCanvas(const std::string& group, const std::string& name, KvThemedPlot* plot) const;
	void applyPalette(const std::string& group, const std::string& name, KvThemedPlot* plot) const;

	// 返回主题group::theme引用的canvas名字，空字符串表示无引用
	std::string canvasName(const std::string& group, const std::string& theme);

	// 返回主题theme引用的layout名字，空字符串表示无引用
	std::string layoutName(const std::string& group, const std::string& theme);

	// 返回主题theme引用的palette名字，空字符串表示无引用
	std::string paletteName(const std::string& group, const std::string& theme);

	// 获取canvas的色带
	colorlist getCanvas(const std::string& group, const std::string& name);

	// 获取palette的色带
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

	void applyLayout_(const std::string& group, const jobject& jobj, KvThemedPlot* plot, bool inTheme) const; // TODO: 是否需要inTheme
	void applyCanvas_(const jvalue& jval, KvThemedPlot* plot) const;
	void applyPalette_(const jvalue& jval, KvThemedPlot* plot) const;

	// 返回true，表示level可见，否则level不可见
	static bool tryVisible_(int level, const jvalue& jval, KvThemedPlot* plot);

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

	static bool isEmpty_(const theme_type& theme);

	const jobject& themeAt_(int type, const std::string& group, const std::string& name) const;

private:
	
	theme_type global_; // 全局主题(未分组)
	std::map<std::string, theme_type> grouped_; // 分组主题

private:
	KsThemeManager() = default;
	~KsThemeManager() = default;

	KsThemeManager(const KsThemeManager & app) = delete;
	void operator=(const KsThemeManager & app) = delete;
};
