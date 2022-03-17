#pragma once
#include <memory>
#include "kDsp.h"

class KcSampled1d;


// 信号乘法，要求两个信号尺度一致

class KgMultiply
{
public:
	KgMultiply(kIndex dim, kIndex channels = 1);

	void process(const KcSampled1d& in, KcSampled1d& out);
	void porcess(KcSampled1d& inout);

private:
	std::unique_ptr<KcSampled1d> sig_; // 与输入相乘的信号
};

