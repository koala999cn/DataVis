#pragma once
#include "KtFraming.h"
#include <cmath>


// 基于插值算法的重采样实现框架

// NOTE: length vs size
// length表示数据组的长度，多通道的数据为一组
// size表示数据长度，=length * channels()

template<typename T>
class KtResampling
{
public:
	KtResampling(unsigned winlen, unsigned chann, double factor)
		: framing_(winlen, chann, 1)
		, factor_(factor) {
		reset();
	}

	unsigned length() const { return framing_.length(); }
	unsigned channels() const { return framing_.channels(); }
	unsigned buffered() const { return framing_.buffered(); }

	// 缓存数据，暂不执行resample操作
	void push(const T* first, const T* last) { 
		framing_.push(first, last); 
	}

	void reset() { 
		framing_.reset(); 
		std::vector<T> buf(length() / 2 * channels(), 0);
		framing_.push(buf.data(), buf.data() + buf.size()); // 压入半窗数据
		obuf_.clear();
		ipos_ = 0, opos_ = 0;
	}


	template<typename INTERP> 
	unsigned apply(const T* in, unsigned ilen, T* out, unsigned olen, INTERP interp);

	template<typename INTERP> 
	unsigned flush(T* out, unsigned olen, INTERP interp);


private:

	// 将obuf_中的数据输出到out，返回写入的数据组长度
	unsigned flushBuf_(T* out, unsigned olen);

	// 再输入inlen组数据，会生成多少组数据？
	unsigned inlenToOutlen_(unsigned insize) const;

	// 要输入多少组数据，才能生成outlen组数据？
	unsigned outlenToInlen_(unsigned outsize) const;


private:

	template<typename INTERP>
	struct KpInterp_
	{
		KpInterp_(KtResampling& rs, INTERP it, T* ob, T* oe)
			: resamp(rs), interp(it), obuf(ob), oend(oe) {}

		INTERP interp;
		KtResampling& resamp;
		T* obuf;
		T* oend; // for debug

		void operator()(const T* ibuf) {
			auto len = resamp.length();
			auto chann = resamp.channels();
			auto mid = len / 2;
			auto& opos = resamp.opos_;
			auto& ipos = resamp.ipos_;
			const auto factor = resamp.factor_;
			for (auto pos = opos * factor; pos <= ipos; pos += factor, opos++) {
				assert(ipos - pos < 1);
				assert(obuf + chann <= oend);
				auto phase = mid + 1 - (ipos - pos);

				// 逐通道插值
				for (unsigned c = 0; c < chann; c++) 
					*obuf++ = interp(ibuf + c, len, chann, phase);
			}

			++ipos;
		}
	};

	friend template<typename INTERP> struct KpInterp_;

private:
	KtFraming<T> framing_;
	double factor_; // 重采样系数，= inrate/outrate，大于1表示降采样，小于1表示升采样
	unsigned ipos_; // 累计输入数
	unsigned opos_; // 累计输出数
	std::vector<T> obuf_;  // 输出缓存
};


template<typename T> template<typename INTERP>
unsigned KtResampling<T>::apply(const T* in, unsigned ilen, T* out, unsigned olen, INTERP interp)
{
	auto chann = channels();

	// 先输出缓存数据
	auto oled = flushBuf_(out, olen); // ol表示已输出的数据长度

	// 写满剩下的out
	auto ilen0 = outlenToInlen_(olen - oled); // 计算需要多少输入才能写满out
	auto itpr = KpInterp_(*this, interp, out + oled, out + olen);
	framing_.apply(in, in + ilen0 * chann, itpr);
	oled = (itpr.obuf - out) / chann; // 重新计算写入out的数据总长度

	// 若还有输入，输出到obuf_
	auto ilen1 = ilen - ilen0;
	auto olen0 = inlenToOutlen_(ilen1);
	obuf_.resize(olen0 * chann);
	itpr.obuf = obuf_.data();
	itpr.oend = obuf_.data() + obuf_.size();
	framing_.apply(in + ilen0 * chann, in + ilen * chann, itpr);

	return oled; // 返回写入out的数据长度
}


template<typename T> template<typename INTERP>
unsigned KtResampling<T>::flush(T* out, unsigned olen, INTERP interp)
{
	// 先输出缓存数据
	auto oled = flushBuf_(out, olen); // ol表示已输出的数据长度

	if (framing_.buffered() > length() / 2) {
		auto paddings = (length() - length() / 2); 
		std::vector<T> zeros(paddings * channels(), 0);
		oled += apply(zeros.data(), paddings, out + oled * channels(), olen - oled, interp);
	}

	return oled;
}


template<typename T>
unsigned KtResampling<T>::flushBuf_(T* out, unsigned olen)
{
	auto bsize = obuf_.size();

	if (bsize) {
		auto osize = olen * chann;
		if (bsize > osize) bsize = osize;

		std::copy(obuf_.cbegin(), obuf_.cbegin() + bsize, out);
		obuf_.erase(obuf_.begin(), obuf_.begin() + bsize);
	}

	return bsize / channels();
}


template<typename T>
unsigned KtResampling<T>::inlenToOutlen_(unsigned inlen) const
{
	// opos <= ipos / factor
	auto opos = static_cast<unsigned>((ipos_ + inlen) / factor_); // 取floor
	return opos - opos_ + obuf_.size() / channels();
}


template<typename T>
unsigned KtResampling<T>::outlenToInlen_(unsigned outlen) const
{
	auto blen = obuf_.size() / channels();
	if (outlen < blen)
		return 0;

	// ipos >= opos * factor
	auto ipos = std::ceil((opos_ + outlen - blen) * factor_)
	return static_cast<unsigned>(ipos) - ipos_;
}
