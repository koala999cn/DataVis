#pragma once
#include "KvDataProvider.h"
#include <memory>

class KvData;


// 数据流的抽象接口，实现逐帧提供数据
class KvDataStream : public KvDataProvider
{
public:
	
	using KvDataProvider::KvDataProvider;

	bool isStream() const final { return true; }

	// 停止数据流动
	// 启动数据流通过重载KvDataProvider::pushData实现
	// 监听数据流通过连接KvDataProvider::onData信号实现
	virtual void stop() = 0;

	// 数据流是否在流动
	virtual bool running() const = 0;
};

