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

	void slotObjectActivated(KvPropertiedObject* obj) {
		emit objectActivated(obj);
	}

	void slotObjectNameChanged(KvPropertiedObject* obj) {
		emit objectNameChanged(obj);
	}

	void slotObjectPropertyChanged(KvPropertiedObject* obj, int propId, const QVariant& newVal) {
		emit objectPropertyChanged(obj, propId, newVal);
	}

	void slotShowInDock(KvPropertiedObject* obj, QWidget* widget);

	void slotCloseDock(KvPropertiedObject* obj);

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

