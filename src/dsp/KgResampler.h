#pragma once
#include "KtResampling.h"
#include "kDsp.h"
#include <vector>
#include <functional>


// 基于KtResampling实现的重采样
// 对于降采样，在KtResampling之后加了一个低通滤波器
// 实现linear、lagrange、sinc三种插值方法
class KgResampler
{
public:
	enum KeMethod
	{
		k_linear,
		k_lagrange,
		k_sinc
	};

	// @factor: 重采样系数 = out-sample-rate / in-sample-rate
	KgResampler(int method, kIndex winlen, kIndex chann, double factor);

	int method() const { return interpMethod_; }
	void setMethod(int md) { interpMethod_ = md; }

	auto length() const { return resamp_.length(); }
	auto channels() const { return resamp_.channels(); }
	auto buffered() const { return resamp_.ibuffered(); }
	auto factor() const { return resamp_.factor(); }

	auto olength(kIndex ilen) const { return resamp_.olength(ilen); }
	auto flength() const { return resamp_.flength(); }


	// 对in执行重采样操作，输出结果到out
	unsigned apply(const kReal* in, kIndex ilen, kReal* out, kIndex olen);
	void apply(const kReal* in, kIndex ilen, std::vector<kReal>& out);
	std::vector<kReal> apply(const kReal* in, kIndex ilen);

	unsigned flush(kReal* out, kIndex olen);
	void flush(std::vector<kReal>& out);
	std::vector<kReal> flush();

	void reset() { resamp_.reset(); }
	void reset(int method, kIndex winlen, kIndex chann, double factor) {
		interpMethod_ = method;
		resamp_.reset(winlen, chann, factor);
	}

private:
	using interp_t = std::function<kReal(const kReal*, double)>;
	interp_t getInterp() const;

private:
	int interpMethod_;
	KtResampling<kReal> resamp_;
};

