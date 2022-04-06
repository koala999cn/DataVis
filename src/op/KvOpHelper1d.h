#pragma once
#include "KvDataOperator.h"


// 为一维操作符提供缺省的processImpl_实现，帮助处理多通道数据和二维framing流
// 继承类只需重载processNaive_方法，实现单一的单通道数据处理即可
class KvOpHelper1d : public KvDataOperator
{
public:
	KvOpHelper1d(const QString& name, KvDataProvider* parent)
		: KvDataOperator(name, parent) {}

protected:

	std::shared_ptr<KvData> processImpl_(std::shared_ptr<KvData> data) override;

	// @in: 单通道数据
	// @len: 输入in的尺寸
	// @out: 输出结果，可能与in重叠，尺寸等于length(dim() - 1)
	virtual void processNaive_(const kReal* in, unsigned len, kReal* out) = 0;
};

