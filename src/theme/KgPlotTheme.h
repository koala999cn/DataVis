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
	QStringList list() const;

	void apply(const QString& theme, QCustomPlot* plot) const;

protected:

	void tryBase_(const QJsonObject& jobj, QCustomPlot* plot) const;

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

	static void tryData_(const QJsonObject& jobj, QCustomPlot* plot);
	static void tryText_(const QJsonObject& jobj, QCustomPlot* plot);
	static void tryMargins_(const QJsonObject& jobj, QCustomPlot* plot);
	static void tryLegend_(const QJsonObject& jobj, QCustomPlot* plot);
	static void trySpacing_(const QJsonObject& jobj, QCustomPlot* plot);

private:

	void removeInvalidThemes_();

private:
	std::map<QString, QJsonObject> themes_;
};
