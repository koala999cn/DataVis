#pragma once
#include <map>
#include <QString>
#include <QJsonObject>

class QCustomPlot;

class KgPlotTheme
{
public:

	// @path: theme�ļ�·����������������ʽ����"theme\*.json"����������Ŀ¼
	bool load(const char* path);

	// �����Ѽ��ص�theme��name���б�
	QStringList listThemes() const;
	QStringList listCanvas() const;
	QStringList listPalettes() const;
	QStringList listLayouts() const;

	void applyTheme(const QString& name, QCustomPlot* plot) const;
	void applyLayout(const QString& name, QCustomPlot* plot) const;
	void applyCanvas(const QString& name, QCustomPlot* plot) const;
	void applyPalette(const QString& name, QCustomPlot* plot) const;

	// ��������theme���õ�canvas���֣����ַ�����ʾ������
	QString canvasName(const QString& theme);

	// ��������theme���õ�layout���֣����ַ�����ʾ������
	QString layoutName(const QString& theme);

	// ��������theme���õ�palette���֣����ַ�����ʾ������
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

	// ����ȫ�����ԣ�Ŀǰ����line��text��title��label
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
