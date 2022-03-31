#pragma once
#include <memory>
#include "kDsp.h"

class KcSampled1d;


// 信号乘法，要求两个信号尺度一致

class KgMultiply
{
public:
	KgMultiply(kIndex nx, kIndex channels = 1);

	void process(const KcSampled1d& in, KcSampled1d& out) const;
	void process(KcSampled1d& inout) const;
	void process(kReal* buf, kIndex channels) const;

	KcSampled1d* operator->() { return sig_.get(); }
	const KcSampled1d* operator->() const { return sig_.get(); }

private:
	std::unique_ptr<KcSampled1d> sig_; // 与输入相乘的信号
};

