#pragma once
#include "KvDataOperator.h"


// 为一维操作符提供缺省的processImpl_实现，帮助处理多通道数据和二维framing流
// 继承类只需重载processNaive_方法，实现单一的单通道数据处理即可
class KvOpSampled1dHelper : public KvDataOperator
{
	using super_ = KvDataOperator;

public:
	KvOpSampled1dHelper(const std::string_view& name, bool splitChannels);

	bool permitInput(int dataSpec, unsigned inPort) const override;

	kIndex size(kIndex outPort, kIndex axis) const override;

	void output() override;

protected:

	// 返回输入数据的尺寸规格，0代表无要求，可处理任意长度输入
	// 缺省实现为输入数据的尺寸，若无输入连接，则返回0
	virtual kIndex isize_() const;

	// 返回is帧出入生成的输出帧数
	// 缺省实现为一致尺寸
	virtual kIndex osize_(kIndex is) const {
		return is;
	}

	// @in: 单通道数据
	// @len: 输入in的尺寸
	// @out: 输出结果，可能与in重叠，尺寸等于size(dim() - 1)
	virtual void op_(const kReal* in, unsigned len, kReal* out) = 0;

	// 创建输出对象的帮助函数，由继承类在onStartPipeline中调用
	void prepareOutput_();

private:

	void output1d_();

	void output2d_();

private:
	bool splitChannels_;
};

