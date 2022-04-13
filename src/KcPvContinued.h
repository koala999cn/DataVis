#pragma once
#include "KcPvSampled.h"

class KvData;

// 连续数据provider
class KcPvContinued : public KcPvSampled
{
public:
	KcPvContinued(const QString& name, std::shared_ptr<KvData> data);

private:
	std::shared_ptr<KvData> data_;
};
