#pragma once
#include "KvPropertiedObject.h"
#include <memory>
#include "dsp/KvData.h" // for kReal & kRange


// 数据源的抽象类

class KvDataProvider : public KvPropertiedObject
{
	Q_OBJECT

public:

	KvDataProvider(const QString& name, KvDataProvider* parent = nullptr) 
		: KvPropertiedObject(name, parent) {}

	virtual bool isStream() const = 0;

	// 数据的维度
	virtual kIndex dim() const = 0;

	virtual kIndex channels() const { return 1; }

	// 返回第axis轴的数据视图范围. 
	virtual kRange range(kIndex axis) const = 0;

	// 返回第axis轴的步进，即dx, dy, dz...
	// 返回=0表示步进未知，返回inf表示步进非均匀
	virtual kReal step(kIndex axis) const = 0;

	virtual kIndex length(kIndex axis) const = 0;

	bool start() {
		emit onStarting();
		bool r = startImpl_();
		onStarted(r);
		return r;
	}

	bool stop() {
		emit onStoping();
		bool r = stopImpl_();
		onStopped(r);
		return r;
	}

	// 数据流是否在流动
	virtual bool running() const = 0;

signals:
	void onStarting();
	void onStarted(bool ok);
	void onStoping();
	void onStopped(bool ok);

	void onData(std::shared_ptr<KvData> data);

private:

	// 开始推送数据
	// 监听数据流通过连接KvDataProvider::onData信号实现
	virtual bool startImpl_() = 0;

	// 停止数据流动
	virtual bool stopImpl_() = 0;
};

