#pragma once
#include "KvDataOperator.h"


// 为一维操作符提供缺省的outputImpl_实现，帮助处理多通道数据和二维framing流
// 继承类只需重载op_方法，实现单一的单通道数据处理即可
class KvOpSampled1dHelper : public KvDataOperator
{
	using super_ = KvDataOperator;

public:
	KvOpSampled1dHelper(const std::string_view& name, bool splitChannels, bool permitSamp2d);

	bool permitInput(int dataSpec, unsigned inPort) const override;

	kIndex size(kIndex outPort, kIndex axis) const override;


protected:

	// 返回输入数据的尺寸规格，0代表无要求，可处理任意长度输入
	// 缺省实现为输入数据的尺寸，若无输入连接，则返回0
	virtual kIndex isize_() const;

	// 返回is帧输入数据生成的输出数据帧数
	// 缺省实现为一致尺寸，即输出长度等于输入长度
	virtual kIndex osize_(kIndex is) const {
		return is;
	}

	// @in: 单通道数据
	// @len: 输入in的尺寸
	// @ch: 当前通道序号（splitChannels_为true时），或通道数（splitChannels_为false时）
	// @out: 输出结果，可能与in重叠，尺寸等于size(dim() - 1)
	virtual void op_(const kReal* in, unsigned len, unsigned ch, kReal* out) = 0;

	// 帮助函数，创建输出对象的帮助函数，由继承类在onStartPipeline中调用
	void createOutputData_();

private:

	void outputImpl_() override;

	void output1d_();

	void output2d_();

private:
	bool splitChannels_; // 有的派生类希望自己处理多通道数据，这里提供1个标记
	bool permitSamp2d_; // 允许处理流式数据？
};

