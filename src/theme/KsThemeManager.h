#pragma once
#include <map>
#include <QString>
#include <QJsonObject>
#include "KtSingleton.h"


#define singletonThemeManager KtSingleton<KsThemeManager, true, true>

class KvThemedPlot;

// 链接json对象与KvThemedPlot接口

class KsThemeManager
{
public:

	// @path: theme文件路径，可以是正则表达式，如"theme\*.json"，但不能是目录
	bool load(const char* path);

	// 返回已加载的各类主题元素列表
	QStringList listGroups() const; // 主题类别
	QStringList listThemes() const; // 主题方案
	QStringList listCanvas() const; // 底布方案
	QStringList listPalettes() const; // 配色方案
	QStringList listLayouts() const; // 布局方案

	void applyTheme(const QString& name, KvThemedPlot* plot) const;
	void applyLayout(const QString& name, KvThemedPlot* plot) const;
	void applyCanvas(const QString& name, KvThemedPlot* plot) const;
	void applyPalette(const QString& name, KvThemedPlot* plot) const;

	// 返回主题theme引用的canvas名字，空字符串表示无引用
	QString canvasName(const QString& theme);

	// 返回主题theme引用的layout名字，空字符串表示无引用
	QString layoutName(const QString& theme);

	// 返回主题theme引用的palette名字，空字符串表示无引用
	QString paletteName(const QString& theme);

protected:

	void tryLoad_(const QJsonObject& jobj);
	void tryObjectOrArray_(const QJsonValue& jval, std::function<void(const QJsonObject&)> fn);
	void tryList_(const QJsonObject& jobj, std::function<void(const QString& key, const QJsonValue&)> fn);

	void tryCanvas_(const QJsonObject& jobj, KvThemedPlot* plot) const;
	void tryPalette_(const QJsonObject& jobj, KvThemedPlot* plot) const;
	void tryLayout_(const QJsonObject& jobj, KvThemedPlot* plot) const;

	void applyLayout_(const QJsonObject& jobj, KvThemedPlot* plot, bool inTheme) const; // TODO: 是否需要inTheme
	void applyCanvas_(const QJsonValue& jval, KvThemedPlot* plot) const;
	    static void applyCanvasBkgnd_(const QJsonValue& jval, KvThemedPlot* plot);
	    static void applyCanvasAxisRect_(const QJsonValue& jval, KvThemedPlot* plot);
	    static void applyCanvasText_(const QJsonValue& jval, KvThemedPlot* plot);
	    static void applyCanvasLine_(const QJsonValue& jval, KvThemedPlot* plot);
	    static void applyCanvasGridline_(const QJsonValue& jval, KvThemedPlot* plot);
	void applyPalette_(const QJsonValue& jval, KvThemedPlot* plot) const;

	// 解析全局属性，目前包括line，text，title，label
	static void tryLevel_(const QJsonObject& jobj, KvThemedPlot* plot, int level);

	static void tryBkgnd_(const QJsonObject& jobj, KvThemedPlot* plot);

	static void tryAxis_(const QJsonObject& jobj, KvThemedPlot* plot);
	    static void applyAxis_(const QJsonValue& jval, KvThemedPlot* plot, int level);
		static void applyTick_(const QJsonValue& jval, KvThemedPlot* plot, int level);

		static void applyAxesLine_(const QJsonValue& jval, KvThemedPlot* plot, int level);
		static void applyAxesBaseline_(const QJsonValue& jval, KvThemedPlot* plot, int level);
	    
	    static void applyAxesSubtick_(const QJsonValue& jval, KvThemedPlot* plot, int level);
		static void applyAxesTitle_(const QJsonValue& jval, KvThemedPlot* plot, int level);
		static void applyAxesLabel_(const QJsonValue& jval, KvThemedPlot* plot, int level); // tick-label

	static void tryGrid_(const QJsonObject& jobj, KvThemedPlot* plot);
	    static void applyGrid_(const QJsonValue& jval, KvThemedPlot* plot, int level);
		static void applyGridLine_(const QJsonValue& jval, KvThemedPlot* plot, int level);
	    static void applyGridMajor_(const QJsonValue& jval, KvThemedPlot* plot, int level);
	    static void applyGridMinor_(const QJsonValue& jval, KvThemedPlot* plot, int level);
	    static void applyGridZeroline_(const QJsonValue& jval, KvThemedPlot* plot, int level);

	static void tryMargins_(const QJsonObject& jobj, KvThemedPlot* plot);
	static void tryLegend_(const QJsonObject& jobj, KvThemedPlot* plot);
	static void trySpacing_(const QJsonObject& jobj, KvThemedPlot* plot);

	static void trySpecial_(const QJsonObject& jobj, KvThemedPlot* plot, int level,
		std::function<void(const QJsonValue&, KvThemedPlot*, int)> op);

	static void applyLine_(int level, const QJsonValue& jval, KvThemedPlot* plot);
	static void applyText_(int level, const QJsonValue& jval, KvThemedPlot* plot);
	static void applyTextColor_(int level, const QJsonValue& jval, KvThemedPlot* plot);

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


	std::map<QString, std::pair<int, QString>> groups_; // int表示类型，取值k_theme, k_canvas, k_palette, k_layout
	std::map<QString, QJsonObject> themes_;
	std::map<QString, QJsonValue> canvas_;
	std::map<QString, QJsonValue> palettes_;
	std::map<QString, QJsonObject> layouts_;


private:
	KsThemeManager() {}

	friend singletonThemeManager;
};


#define kThemeManager singletonThemeManager::instance().get()