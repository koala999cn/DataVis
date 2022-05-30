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
	KgResampler(int method, kIndex wsize, kIndex chann, double factor);

	int method() const { return interpMethod_; }
	void setMethod(int md) { interpMethod_ = md; }

	auto size() const { return resamp_.size(); }
	auto channels() const { return resamp_.channels(); }
	auto buffered() const { return resamp_.ibuffered(); }
	auto factor() const { return resamp_.factor(); }

	auto osize(kIndex isize) const { return resamp_.osize(isize); }
	auto fsize() const { return resamp_.fsize(); }


	// 对in执行重采样操作，输出结果到out
	unsigned apply(const kReal* in, kIndex isize, kReal* out, kIndex osize);
	void apply(const kReal* in, kIndex isize, std::vector<kReal>& out);
	std::vector<kReal> apply(const kReal* in, kIndex isize);

	unsigned flush(kReal* out, kIndex osize);
	void flush(std::vector<kReal>& out);
	std::vector<kReal> flush();

	void reset() { resamp_.reset(); }
	void reset(int method, kIndex wsize, kIndex chann, double factor) {
		interpMethod_ = method;
		resamp_.reset(wsize, chann, factor);
	}

private:
	using interp_t = std::function<kReal(const kReal*, double)>;
	interp_t getInterp() const;

private:
	int interpMethod_;
	KtResampling<kReal> resamp_;
};

