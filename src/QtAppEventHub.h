#pragma once
#include "KtSingleton.h"
#include <QObject>
#include <QVariant>


#define singletonAppEventHub KtSingleton<QtAppEventHub, true, true>

class KvPropertiedObject;
class QWidget;
class QWindow;

// 应用程序的全局信号转发中枢
class QtAppEventHub : public QObject
{
	Q_OBJECT

public slots:

	// 将obj的widget显示到dock
	void showDock(KvPropertiedObject* obj, QWidget* widget);

	// 关闭与对象obj关联的dock
	void closeDock(KvPropertiedObject* obj);

	// 是否由dock与obj关联
	bool isDocked(const KvPropertiedObject* obj);

	// 重新加载属性页面，用于支持属性项的动态调整
	void refreshPropertySheet();

	bool startPipeline(KvPropertiedObject* root);

	void stopPipeline(KvPropertiedObject* root);

signals:

	void objectActivated(KvPropertiedObject* obj);

	void objectNameChanged(KvPropertiedObject* obj);

	// 通知属性页面更新刷新值
	void objectPropertyChanged(KvPropertiedObject* obj, int propId, const QVariant& newVal);

	void dockClosed(KvPropertiedObject* obj);

	void pipelineStarting(KvPropertiedObject* root);
	
	void pipelineStarted(KvPropertiedObject* root, bool ok);

	void pipelineStopping(KvPropertiedObject* root);

	void pipelineStopped(KvPropertiedObject* root);


private:
	QtAppEventHub() {}

	friend singletonAppEventHub;
};

#define kAppEventHub singletonAppEventHub::instance().get()

