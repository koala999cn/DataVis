#pragma once
#include "KtSingleton.h"
#include <QObject>
#include <QVariant>


#define singletonAppEventHub KtSingleton<QtAppEventHub, true, true>

class KvPropertiedObject;
class QWidget;


// 应用程序的全局信号转发中枢
class QtAppEventHub : public QObject
{
	Q_OBJECT

public slots:

	// 将obj的widget显示到dock
	void showDock(KvPropertiedObject* obj, QWidget* widget);
	
	// 关闭与对象obj关联的dock
	void closeDock(KvPropertiedObject* obj);

signals:

	void objectActivated(KvPropertiedObject* obj);

	void objectNameChanged(KvPropertiedObject* obj);

	void objectPropertyChanged(KvPropertiedObject* obj, int propId, const QVariant& newVal);

	void dockClosed(KvPropertiedObject* obj);


private:
	QtAppEventHub() {}

	friend singletonAppEventHub;
};

#define kAppEventHub singletonAppEventHub::instance().get()

