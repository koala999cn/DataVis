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

	virtual kIndex length(kIndex axis) const {
		return range(axis).length() / step(axis);
	}


	// 发送onData信号
	virtual bool pushData() = 0;

signals:
	void onData(std::shared_ptr<KvData> data);
};

