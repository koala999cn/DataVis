#pragma once
#include "KtFraming.h"
#include <cmath>


// 基于插值算法的重采样实现框架，主要利用KtFraming提供流数据插值环境
// 每次调用插值算子时，确保插值点位于插值窗口的中间，具体位于区间(N/2, N/2+1]，
// 其中N为插值窗长，即参与插值的数据点数量
//   - 当N为偶数时，插值点的左边和右边各有n个点
//   - 当N为奇数时，插值点的左边有n个点，右边有n+1个点
// 
// NOTE: length vs size
// length表示帧长，1帧包括channels个数据
// size表示数据尺寸，= length * channels

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

	// 未处理的输入帧数
	unsigned buffered() const { 
		assert(framing_.buffered() >= mid_());
		return framing_.buffered() - mid_(); 
	}

	auto factor() const { return factor_; }


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
		ipos_ = 0, opos_ = 0;
	}

	// 重置
	void reset(unsigned winlen, unsigned chann, double factor) {
		framing_.reset(winlen, chann, 1);
		factor_ = factor;
		reset();
	}


	template<typename INTERP> 
	unsigned apply(const T* in, unsigned ilen, T* out, unsigned olen, INTERP interp);

	template<typename INTERP> 
	unsigned flush(T* out, unsigned olen, INTERP interp);

	// 若输入ilen帧数据，将输出多少帧数据
	unsigned olength(unsigned ilen) const;

	// 若生成olen帧数据，需要多少帧输入？
	unsigned ilength(unsigned olen) const;

	// flush将输出多少帧数据
	unsigned flength() const;

private:

	unsigned mid_() const { return length() / 2; }

	// 将obuf_中的数据输出到out，返回写入的帧数
	unsigned flushObuf_(T* out, unsigned olen);

private:

	template<typename INTERP>
	struct KpInterpWrap_
	{
		KpInterpWrap_(KtResampling& rs, INTERP it, T* ob, T* oe)
			: resamp(rs), interp(it), obuf(ob), oend(oe) {}

		INTERP interp;
		KtResampling& resamp;
		T* obuf;
		T* oend; // for debug

		void operator()(const T* ibuf) {
			auto len = resamp.length();
			auto chann = resamp.channels();
			auto mid = resamp.mid_();
			auto& opos = resamp.opos_;
			auto& ipos = resamp.ipos_;
			const auto factor = resamp.factor_;
			for (auto pos = opos * factor; pos <= ipos; pos += factor, opos++) {
				assert(ipos - pos < 1);
				assert(obuf + chann < oend);
				auto phase = mid + 1 - (ipos - pos);

				// 逐通道插值
				for (unsigned c = 0; c < chann; c++) 
					*obuf++ = interp(ibuf + c, phase);
			}

			++ipos;
		}
	};

	template<typename INTERP> friend struct KpInterpWrap_;

private:
	KtFraming<T> framing_;
	double factor_; // 重采样系数，= irate/orate，大于1表示降采样，小于1表示升采样
	unsigned ipos_; // 累计输入帧数
	unsigned opos_; // 累计输出帧数
	std::vector<T> obuf_;  // 输出缓存。当用户提供的输出容量不足时，多余的输出将暂存此处。
};


template<typename T> template<typename INTERP>
unsigned KtResampling<T>::apply(const T* in, unsigned ilen, T* out, unsigned olen, INTERP interp)
{
	auto chann = channels();
	auto opos = opos_;

	// 拷贝输出缓存obuf_数据到out
	auto oled = flushObuf_(out, olen); // ol表示已输出的帧数

	// 写满剩下的out
	auto ilen0 = ilength(olen - oled); // 计算需要多少帧输入才能写满out
	if (ilen0 > ilen) ilen0 = ilen;
	auto interpWrap = KpInterpWrap_(*this, interp, out + oled, out + olen);
	framing_.apply(in, in + ilen0 * chann, interpWrap);
	oled += opos_ - opos; // 统计写入out的帧数

	// 若还有输入，输出到obuf_
	auto ilen1 = ilen - ilen0;
	if (ilen1 > 0) {
		auto olen0 = olength(ilen1);
		obuf_.resize(olen0 * chann);
		itpr.obuf = obuf_.data();
		itpr.oend = obuf_.data() + obuf_.size();
		framing_.apply(in + ilen0 * chann, in + ilen * chann, itpr);
	}

	return oled; // 返回写入out的帧数
}


template<typename T> template<typename INTERP>
unsigned KtResampling<T>::flush(T* out, unsigned olen, INTERP interp)
{
	// 拷贝输出缓存obuf_数据到out
	auto oled = flushObuf_(out, olen); // ol表示已输出的帧数

	if (buffered() > 0) {
		auto paddings = (length() - mid_()); 
		std::vector<T> zeros(paddings * channels(), 0);
		oled += apply(zeros.data(), paddings, out + oled * channels(), olen - oled, interp);
	}

	return oled;
}


template<typename T>
unsigned KtResampling<T>::olength(unsigned ilen) const
{
	assert(opos_ <= ipos_ / factor_);

	auto opos = static_cast<unsigned>((ipos_ + ilen) / factor_); // 取floor
	return opos - opos_ + obuf_.size() / channels();
}


template<typename T>
unsigned KtResampling<T>::ilength(unsigned olen) const
{
	auto blen = obuf_.size() / channels();
	if (olen < blen)
		return 0;

	assert(ipos_ >= opos_ * factor_);
	auto ipos = std::ceil((opos_ + olen - blen) * factor_);
	return static_cast<unsigned>(ipos) - ipos_;
}


template<typename T>
unsigned KtResampling<T>::flength() const
{
	return olength(buffered() > 0 ? length() - mid_() : 0);
}


template<typename T>
unsigned KtResampling<T>::flushObuf_(T* out, unsigned olen)
{
	auto bsize = obuf_.size();

	if (bsize > 0) {
		auto osize = olen * channels();
		if (bsize > osize) bsize = osize;

		std::copy(obuf_.cbegin(), obuf_.cbegin() + bsize, out);
		obuf_.erase(obuf_.begin(), obuf_.begin() + bsize);
	}

	return bsize / channels();
}
