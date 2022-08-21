#pragma once
#include <map>
#include <QString>
#include <QJsonObject>
#include "KtSingleton.h"


#define singletonThemeManager KtSingleton<KsThemeManager, true, true>

class KvThemedPlot;

// ����json������KvThemedPlot�ӿ�

class KsThemeManager
{
public:

	// @path: theme�ļ�·����������������ʽ����"theme\*.json"����������Ŀ¼
	bool load(const char* path);

	// �����Ѽ��صĸ�������Ԫ���б�
	QStringList listGroups() const; // �������
	QStringList listThemes() const; // ���ⷽ��
	QStringList listCanvas() const; // �ײ�����
	QStringList listPalettes() const; // ��ɫ����
	QStringList listLayouts() const; // ���ַ���

	void applyTheme(const QString& name, KvThemedPlot* plot) const;
	void applyLayout(const QString& name, KvThemedPlot* plot) const;
	void applyCanvas(const QString& name, KvThemedPlot* plot) const;
	void applyPalette(const QString& name, KvThemedPlot* plot) const;

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

	void tryCanvas_(const QJsonObject& jobj, KvThemedPlot* plot) const;
	void tryPalette_(const QJsonObject& jobj, KvThemedPlot* plot) const;
	void tryLayout_(const QJsonObject& jobj, KvThemedPlot* plot) const;

	void applyLayout_(const QJsonObject& jobj, KvThemedPlot* plot, bool inTheme) const; // TODO: �Ƿ���ҪinTheme
	void applyCanvas_(const QJsonValue& jval, KvThemedPlot* plot) const;
	void applyPalette_(const QJsonValue& jval, KvThemedPlot* plot) const;

	// ����0��ʾΪ���δ����applyVisible
	// ����1��ʾ������applyVisible(level, true)
	// ����-1��ʾ������applyVisible(level, false)
	static int tryVisible_(int level, const QJsonValue& jval, KvThemedPlot* plot);

	// ����ȫ�����ԣ�Ŀǰ����line��text��title��label
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

	// ���jobj�Ƿ��������propName�������������enterLevel(level, propId)������һlevel�����ص�op
	// @newLevel: enterLevel(level, propId)�ķ��ؽ��
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


	std::map<QString, std::pair<int, QString>> groups_; // int��ʾ���ͣ�ȡֵk_theme, k_canvas, k_palette, k_layout
	std::map<QString, QJsonObject> themes_;
	std::map<QString, QJsonValue> canvas_;
	std::map<QString, QJsonValue> palettes_;
	std::map<QString, QJsonObject> layouts_;


private:
	KsThemeManager() {}

	friend singletonThemeManager;
};


#define kThemeManager singletonThemeManager::instance().get()