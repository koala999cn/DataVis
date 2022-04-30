#pragma once
#include <vector>
#include "KtFraming.h"


// FIR滤波器的模板类
// @TD: 输入输出数据的类型
// @TC: 滤波器系数的类型

template<typename TD, typename TC = TD>
class KtFIR
{
public:
	// 单通道，每次平移1个数据
	KtFIR(std::vector<TC>&& h, unsigned channels = 1) : h_(std::move(h)), buf_(h.size(), channels, 1, 0) {}
	KtFIR(const std::vector<TC>& h, unsigned channels = 1) : h_(h), buf_(h.size(), channels, 1, 0) {}


	// 滤波器阶数
	auto order() const { return h_.size() - 1; }

	// 滤波器抽头数
	auto taps() const { return h_.size(); }

	unsigned channels() const { return buf_.channels(); }


	// 对输入in执行滤波操作，结果存储到out，返回写入的数据量
	// out的尺寸不小于count * channels
	unsigned apply(const TD* in, unsigned count, TD* out);

	// 对缓存数据执行滤波操作，结果存储到out，返回写入的数据量
	// out的尺寸不小于count * channels
	unsigned flush(TD* out);


private:
	struct KpFilter
	{
		KpFilter(std::vector<TC>& h_, TD*& out_, unsigned ch_) : h(h_), out(out_), ch(ch_) {}

		void operator()(const TD* data) {
			for (unsigned c = 0; c < ch; c++) {
				TD v{0};
				const TD* dp = data + c;
				const TC* hp = h.data();
				for (unsigned i = 0; i < h.size(); i++) {
					v += *dp * *hp++; // 点积
					dp += ch;
				}
				*out++ = v;
			}
		}

		std::vector<TC>& h;
		TD*& out;
		unsigned ch;
	};

private:
	std::vector<TC> h_; // 滤波系数
	KtFraming<TD> buf_;
};


template<typename TD, typename TC>
unsigned KtFIR<TD, TC>::apply(const TD* in, unsigned count, TD* out)
{
	auto buf = out;
	KpFilter op(h_, buf, channels());
	buf_.apply(in, in + count, op);
	return (buf - out) / channels();
}


template<typename TD, typename TC>
unsigned KtFIR<TD, TC>::flush(TD* out)
{
	auto buf = out;
	KpFilter op(h_, buf, channels());
	buf_.flush(op);
	return (buf - out) / channels();
}

