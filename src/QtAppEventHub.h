#pragma once
#include "KtSingleton.h"
#include <QObject>
#include <QVariant>

#define singletonAppEventHub KtSingleton<QtAppEventHub, true, true>

class KvInputSource;
class KvOutputDevice;


// 应用程序的全局信号转发中枢
class QtAppEventHub : public QObject
{
	Q_OBJECT

public slots:

	void slotInputSourceActivated(KvInputSource* input, bool active) {
		emit onInputSourceActivated(input, active);
	}
	void slotInputSourcePropertyChanged(KvInputSource* input, int propId, const QVariant& newVal) {
		emit onInputSourcePropertyChanged(input, propId, newVal);
	}

	void slotOutputDeviceActivated(KvOutputDevice* output, bool active) {
		emit onOutputDeviceActivated(output, active);
	}
	void slotOutputDevicePropertyChanged(KvOutputDevice* output, int propId, const QVariant& newVal) {
		emit onOutputDevicePropertyChanged(output, propId, newVal);
	}


signals:

	void onInputSourceActivated(KvInputSource* input, bool active);
	void onInputSourcePropertyChanged(KvInputSource* input, int propId, const QVariant& newVal);

	void onOutputDeviceActivated(KvOutputDevice* output, bool active);
	void onOutputDevicePropertyChanged(KvOutputDevice* output, int propId, const QVariant& newVal);

private:
	QtAppEventHub() {}

	friend singletonAppEventHub;
};

#define kAppEventHub singletonAppEventHub::instance().get()

