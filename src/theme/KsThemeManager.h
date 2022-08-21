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
	void applyPalette_(const QJsonValue& jval, KvThemedPlot* plot) const;

	// 返回0表示为检测未调用applyVisible
	// 返回1表示调用了applyVisible(level, true)
	// 返回-1表示调用了applyVisible(level, false)
	static int tryVisible_(int level, const QJsonValue& jval, KvThemedPlot* plot);

	// 解析全局属性，目前包括line，text，title，label
	static void tryLevel_(int level, const QJsonObject& jobj, KvThemedPlot* plot);

	static void tryBkgnd_(int level, const QJsonObject& jobj, KvThemedPlot* plot);

	static void tryAxis_(const QJsonObject& jobj, KvThemedPlot* plot);
	static void applyAxis_(int level, const QJsonValue& jval, KvThemedPlot* plot);
	static void applyTick_(int level, const QJsonValue& jval, KvThemedPlot* plot);

	static void tryGrid_(const QJsonObject& jobj, KvThemedPlot* plot);
	static void applyGrid_(int level, const QJsonValue& jval, KvThemedPlot* plot);

	static void tryMargins_(int level, const QJsonObject& jobj, KvThemedPlot* plot);
	static void tryLegend_(const QJsonObject& jobj, KvThemedPlot* plot);

	static void trySpecials_(int level, const QJsonObject& jobj, KvThemedPlot* plot,
		std::function<void(int, const QJsonValue&, KvThemedPlot*)> op);

	// 检测jobj是否包含属性propName，若包含则调用enterLevel(level, propId)进入下一level，并回调op
	// @newLevel: enterLevel(level, propId)的返回结果
	// @jval: jobj[propName]
	static void trySpecialProp_(int level, const QJsonObject& jobj, const char* propName, int propId,
		std::function<void(int newLevel, const QJsonValue& jval)> op);

	static void applyFill_(int level, const QJsonValue& jval, KvThemedPlot* plot);
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