#pragma once
#include "KvInputSource.h"
#include "KvData.h"
#include <memory>


// 流输入的抽象接口
class KvInputStream : public KvInputSource
{
	Q_OBJECT

public:
	
	KvInputStream(const QString& name) : KvInputSource(name, true) {}

	virtual bool start() = 0;
	virtual void stop() = 0;
	virtual bool running() const = 0;

signals:

	void onStreamData(std::shared_ptr<KvData> data);
};

