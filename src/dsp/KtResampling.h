#pragma once
#include "KtFraming.h"
#include <cmath>


// 基于插值算法的重采样实现框架，主要利用KtFraming提供流数据插值环境
// 每次调用插值算子时，确保插值点位于插值窗口的中间，具体位于区间(N/2 - 1, N/2]，
// 其中N为插值窗长，即参与插值的数据点数量
//   - 当N为偶数时，插值点的左边和右边各有n个点
//   - 当N为奇数时，插值点的左边有n个点，右边有n+1个点

template<typename T>
class KtResampling
{
public:
	// @factor: 重采样系数 = out-sample-rate / in-sample-rate
	//          大于1表示升采样，小于1表示降采样
	KtResampling(unsigned wsize, unsigned chann, double factor)
		: framing_(wsize, chann, 1)
		, factorR_(1.0 / factor) {
		reset();
	}

	unsigned size() const { return framing_.size(); }
	unsigned channels() const { return framing_.channels(); }

	// 未处理的输入帧数
	unsigned ibuffered() const { 
		assert(framing_.buffered() >= mid_());
		return framing_.buffered() - mid_(); 
	}

	unsigned obuffered() const {
		return obuf_.size() / channels();
	}

	// 重采样系数
	auto factor() const { return 1.0 / factorR_; }

	// 共处理多少帧输入？
	auto itotal() const { return ipos_; }

	// 共输出多少帧数据？包括输出到obuf_的数据
	auto ototal() const { return opos_ - 1; }


	// 缓存输入，暂不执行resample操作
	void push(const T* first, const T* last) { 
		framing_.push(first, last); 
	}

	// 恢复初始状态
	void reset() { 
		framing_.reset(); 
		std::vector<T> buf(mid_() * channels(), 0);
		framing_.push(buf.data(), buf.data() + buf.size()); // 压入半窗数据
		obuf_.clear();
		ipos_ = 0, opos_ = 1;
	}

	// 重置
	// @wsize: 窗长
	void reset(unsigned wsize, unsigned chann, double factor) {
		framing_.reset(wsize, chann, 1);
		factorR_ = 1.0 / factor;
		reset();
	}


	template<typename INTERP> 
	unsigned apply(const T* in, unsigned isize, T* out, unsigned osize, INTERP interp);

	template<typename INTERP> 
	unsigned flush(T* out, unsigned osize, INTERP interp);

	// 若输入isize帧数据，将输出多少帧数据
	unsigned osize(unsigned isize) const;

	// 若生成osize帧数据，需要多少帧输入？
	unsigned isize(unsigned osize) const;

	// flush将输出多少帧数据
	unsigned fsize() const;

private:

	unsigned mid_() const { return size() / 2; }

	unsigned unbuffered_() const {
		return framing_.size() - framing_.buffered();
	}

	// 将obuf_中的数据输出到out，返回写入的帧数
	// @osize: size fo out
	unsigned flushObuf_(T* out, unsigned osize);

private:

	template<typename INTERP>
	struct KpInterpWrap_
	{
		KpInterpWrap_(KtResampling& rs, INTERP it, T* ostart, T* oend)
			: resamp(rs), interp(it), outp(ostart), oute(oend) {}

		INTERP interp;
		KtResampling& resamp;
		T* outp;
		T* oute; // for debug

		void operator()(const T* ibuf) {
			const auto chann = resamp.channels();
			const auto mid = resamp.mid_();
			const auto factorR = resamp.factorR_;
			auto& opos = resamp.opos_;
			auto& ipos = ++resamp.ipos_;
			
			for (auto pos = opos * factorR; pos <= ipos; pos = ++opos * factorR) {
				assert(ipos - pos < 1);
				assert(outp + chann <= oute);
				auto phase = mid - (ipos - pos);
				// 断言phase位于插值窗的中间
				assert(phase > resamp.mid_() - 1 && phase <= resamp.mid_()); 

				// 逐通道插值
				for (unsigned c = 0; c < chann; c++) 
					*outp++ = interp(ibuf + c, phase);	
			}
		}
	};

	template<typename INTERP> friend struct KpInterpWrap_;

private:
	KtFraming<T> framing_;
	double factorR_; // 为方便计算，取重采样系数的倒数
	unsigned ipos_; // 累计输入帧数
	unsigned opos_; // 累计输出帧数
	std::vector<T> obuf_;  // 输出缓存。当用户提供的输出容量不足时，多余的输出将暂存此处。
};


template<typename T> template<typename INTERP>
unsigned KtResampling<T>::apply(const T* in, unsigned iframes, T* out, unsigned oframes, INTERP interp)
{
	const auto chann = channels();
	
	// 拷贝输出缓存obuf_数据到out
	auto oled = flushObuf_(out, oframes); // oled表示已输出的帧数

	// 写满剩下的out
	auto iframes0 = isize(oframes - oled); // 计算需要多少帧输入才能写满out
	if (iframes0 > iframes) iframes0 = iframes;
	auto interpWrap = KpInterpWrap_(*this, interp, out + oled * chann, out + oframes * chann);
	const auto opos = opos_;
	framing_.apply(in, in + iframes0 * chann, interpWrap);
	oled += opos_ - opos; // 统计写入out的帧数

	// 若还有输入，输出到obuf_
	auto iframes1 = iframes - iframes0;
	if (iframes1 > 0) {
		obuf_.resize(osize(iframes1) * chann);
		interpWrap.outp = obuf_.data();
		interpWrap.oute = obuf_.data() + obuf_.size();
		framing_.apply(in + iframes0 * chann, in + iframes * chann, interpWrap);
	}

	return oled; // 返回写入out的帧数
}


template<typename T> template<typename INTERP>
unsigned KtResampling<T>::flush(T* out, unsigned osize, INTERP interp)
{
	// 拷贝输出缓存obuf_数据到out
	auto oled = flushObuf_(out, osize); // ol表示已输出的帧数

	if (ibuffered() > 0) {
		auto paddings = (size() - mid_()); 
		std::vector<T> zeros(paddings * channels(), 0);
		oled += apply(zeros.data(), paddings, out + oled * channels(), osize - oled, interp);
	}

	return oled;
}


template<typename T>
unsigned KtResampling<T>::osize(unsigned isize) const
{
	assert(ototal() <= itotal() * factor());
	if (isize < unbuffered_())
		return 0;
	isize -= unbuffered_() - 1;

	auto opos = static_cast<unsigned>((itotal() + isize) / factorR_); // 取floor
	return opos - ototal() + obuffered();
}


template<typename T>
unsigned KtResampling<T>::isize(unsigned osize) const
{
	auto bsize = obuffered();
	if (osize <= bsize)
		return 0;

	assert(itotal() >= ototal() / factor());
	auto ipos = std::ceil((ototal() + osize - bsize) * factorR_);
	return static_cast<unsigned>(ipos) - itotal() + unbuffered_() - 1;
}


template<typename T>
unsigned KtResampling<T>::fsize() const
{
	return osize(ibuffered() > 0 ? size() - mid_() : 0);
}


template<typename T>
unsigned KtResampling<T>::flushObuf_(T* out, unsigned oframes)
{
	auto bcount = obuf_.size();

	if (bcount > 0) {
		auto ocount = oframes * channels();
		if (bcount > ocount) bcount = ocount;

		std::copy(obuf_.cbegin(), obuf_.cbegin() + bcount, out);
		obuf_.erase(obuf_.begin(), obuf_.begin() + bcount);
	}

	return bcount / channels();
}
