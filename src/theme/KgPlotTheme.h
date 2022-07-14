#pragma once
#include <map>
#include <QString>
#include <QJsonObject>

class QCustomPlot;

class KgPlotTheme
{
public:

	// @path: theme文件路径，可以是正则表达式，如"theme\*.json"，但不能是目录
	bool load(const char* path);

	// 返回已加载的theme（name）列表
	QStringList listThemes() const;
	QStringList listCanvas() const;
	QStringList listPalettes() const;
	QStringList listLayouts() const;

	void applyTheme(const QString& name, QCustomPlot* plot) const;
	void applyLayout(const QString& name, QCustomPlot* plot) const;
	void applyCanvas(const QString& name, QCustomPlot* plot) const;
	void applyPalette(const QString& name, QCustomPlot* plot) const;

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

	void tryCanvas_(const QJsonObject& jobj, QCustomPlot* plot) const;
	void tryPalette_(const QJsonObject& jobj, QCustomPlot* plot) const;
	void tryLayout_(const QJsonObject& jobj, QCustomPlot* plot) const;

	void applyLayout_(const QJsonObject& jobj, QCustomPlot* plot, bool inTheme) const;
	void applyCanvas_(const QJsonValue& jval, QCustomPlot* plot) const;
	    static void applyCanvasBkgnd_(const QJsonValue& jval, QCustomPlot* plot);
	    static void applyCanvasAxisRect_(const QJsonValue& jval, QCustomPlot* plot);
	    static void applyCanvasText_(const QJsonValue& jval, QCustomPlot* plot);
	    static void applyCanvasLine_(const QJsonValue& jval, QCustomPlot* plot);
	    static void applyCanvasGridline_(const QJsonValue& jval, QCustomPlot* plot);
	void applyPalette_(const QJsonValue& jval, QCustomPlot* plot) const;

	// 解析全局属性，目前包括line，text，title，label
	static void tryGlobal_(const QJsonObject& jobj, QCustomPlot* plot);

	static void tryBkgnd_(const QJsonObject& jobj, QCustomPlot* plot);

	static void tryAxes_(const QJsonObject& jobj, QCustomPlot* plot);
	    static void applyAxes_(const QJsonValue& jval, QCustomPlot* plot, int level);
		static void applyAxesLine_(const QJsonValue& jval, QCustomPlot* plot, int level);
		static void applyAxesBaseline_(const QJsonValue& jval, QCustomPlot* plot, int level);
	    static void applyAxesTick_(const QJsonValue& jval, QCustomPlot* plot, int level);
	    static void applyAxesSubtick_(const QJsonValue& jval, QCustomPlot* plot, int level);
		static void applyAxesTitle_(const QJsonValue& jval, QCustomPlot* plot, int level);
		static void applyAxesLabel_(const QJsonValue& jval, QCustomPlot* plot, int level); // tick-label

	static void tryGrid_(const QJsonObject& jobj, QCustomPlot* plot);
	    static void applyGrid_(const QJsonValue& jval, QCustomPlot* plot, int level);
		static void applyGridLine_(const QJsonValue& jval, QCustomPlot* plot, int level);
	    static void applyGridMajor_(const QJsonValue& jval, QCustomPlot* plot, int level);
	    static void applyGridMinor_(const QJsonValue& jval, QCustomPlot* plot, int level);
	    static void applyGridZeroline_(const QJsonValue& jval, QCustomPlot* plot, int level);

	static void tryMargins_(const QJsonObject& jobj, QCustomPlot* plot);
	static void tryLegend_(const QJsonObject& jobj, QCustomPlot* plot);
	static void trySpacing_(const QJsonObject& jobj, QCustomPlot* plot);

private:

	void removeInvalidThemes_();

private:
	std::map<QString, QJsonObject> themes_;
	std::map<QString, QJsonValue> canvas_;
	std::map<QString, QJsonValue> palettes_;
	std::map<QString, QJsonObject> layouts_;
};
