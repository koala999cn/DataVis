#pragma once
#include "KvPropertiedObject.h"
#include <memory>


class KvInputSnapshot; // 快照输入，全部数据可访问
class KvInputStream; // 流输入，逐帧获取数据


// 输入源的抽象类

class KvInputSource : public KvPropertiedObject
{
public:

	KvInputSource(const QString& name, bool streaming)
		: KvPropertiedObject(name)
	    , streaming_(streaming) {}

	bool isStream() const { return streaming_; }

private:
	bool streaming_;
};

