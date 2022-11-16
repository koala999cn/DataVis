#pragma once
#include "KvDataOperator.h"


// 为一维操作符提供缺省的processImpl_实现，帮助处理多通道数据和二维framing流
// 继承类只需重载processNaive_方法，实现单一的单通道数据处理即可
class KvOpSampled1dHelper : public KvDataOperator
{
public:
	KvOpSampled1dHelper(const std::string_view& name)
		: KvDataOperator(name) {}

	void output() override;

protected:

	// 返回输入数据的尺寸规格，0代表无要求，可处理任意长度输入
	virtual kIndex isize_() const = 0;

	// 返回is帧出入生成的输出帧数
	virtual kIndex osize_(kIndex is) const = 0;

	// @in: 单通道数据
	// @len: 输入in的尺寸
	// @out: 输出结果，可能与in重叠，尺寸等于size(dim() - 1)
	virtual void op_(const kReal* in, unsigned len, kReal* out) = 0;

	// 创建输出对象的帮助函数，由继承类在onStartPipeline中调用
	void prepareOutput_();

private:

	void output1d_();

	void output2d_();

};

