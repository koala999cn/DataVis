#pragma once
#include "KtFraming.h"
#include <cmath>


// ���ڲ�ֵ�㷨���ز���ʵ�ֿ�ܣ���Ҫ����KtFraming�ṩ�����ݲ�ֵ����
// ÿ�ε��ò�ֵ����ʱ��ȷ����ֵ��λ�ڲ�ֵ���ڵ��м䣬����λ������(N/2, N/2+1]��
// ����NΪ��ֵ�������������ֵ�����ݵ�����
//   - ��NΪż��ʱ����ֵ�����ߺ��ұ߸���n����
//   - ��NΪ����ʱ����ֵ��������n���㣬�ұ���n+1����
// 
// NOTE: length vs size
// length��ʾ֡����1֡����channels������
// size��ʾ���ݳߴ磬= length * channels

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

	// δ���������֡��
	unsigned buffered() const { 
		assert(framing_.buffered() >= mid_());
		return framing_.buffered() - mid_(); 
	}

	auto factor() const { return factor_; }


	// �������룬�ݲ�ִ��resample����
	void push(const T* first, const T* last) { 
		framing_.push(first, last); 
	}

	// �ָ���ʼ״̬
	void reset() { 
		framing_.reset(); 
		std::vector<T> buf(mid_() * channels(), 0);
		framing_.push(buf.data(), buf.data() + buf.size()); // ѹ��봰����
		obuf_.clear();
		ipos_ = 0, opos_ = 0;
	}

	// ����
	void reset(unsigned winlen, unsigned chann, double factor) {
		framing_.reset(winlen, chann, 1);
		factor_ = factor;
		reset();
	}


	template<typename INTERP> 
	unsigned apply(const T* in, unsigned ilen, T* out, unsigned olen, INTERP interp);

	template<typename INTERP> 
	unsigned flush(T* out, unsigned olen, INTERP interp);

	// ������ilen֡���ݣ����������֡����
	unsigned olength(unsigned ilen) const;

	// ������olen֡���ݣ���Ҫ����֡���룿
	unsigned ilength(unsigned olen) const;

	// flush���������֡����
	unsigned flength() const;

private:

	unsigned mid_() const { return length() / 2; }

	// ��obuf_�е����������out������д���֡��
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

				// ��ͨ����ֵ
				for (unsigned c = 0; c < chann; c++) 
					*obuf++ = interp(ibuf + c, phase);
			}

			++ipos;
		}
	};

	template<typename INTERP> friend struct KpInterpWrap_;

private:
	KtFraming<T> framing_;
	double factor_; // �ز���ϵ����= irate/orate������1��ʾ��������С��1��ʾ������
	unsigned ipos_; // �ۼ�����֡��
	unsigned opos_; // �ۼ����֡��
	std::vector<T> obuf_;  // ������档���û��ṩ�������������ʱ�������������ݴ�˴���
};


template<typename T> template<typename INTERP>
unsigned KtResampling<T>::apply(const T* in, unsigned ilen, T* out, unsigned olen, INTERP interp)
{
	auto chann = channels();
	auto opos = opos_;

	// �����������obuf_���ݵ�out
	auto oled = flushObuf_(out, olen); // ol��ʾ�������֡��

	// д��ʣ�µ�out
	auto ilen0 = ilength(olen - oled); // ������Ҫ����֡�������д��out
	if (ilen0 > ilen) ilen0 = ilen;
	auto interpWrap = KpInterpWrap_(*this, interp, out + oled, out + olen);
	framing_.apply(in, in + ilen0 * chann, interpWrap);
	oled += opos_ - opos; // ͳ��д��out��֡��

	// ���������룬�����obuf_
	auto ilen1 = ilen - ilen0;
	if (ilen1 > 0) {
		auto olen0 = olength(ilen1);
		obuf_.resize(olen0 * chann);
		itpr.obuf = obuf_.data();
		itpr.oend = obuf_.data() + obuf_.size();
		framing_.apply(in + ilen0 * chann, in + ilen * chann, itpr);
	}

	return oled; // ����д��out��֡��
}


template<typename T> template<typename INTERP>
unsigned KtResampling<T>::flush(T* out, unsigned olen, INTERP interp)
{
	// �����������obuf_���ݵ�out
	auto oled = flushObuf_(out, olen); // ol��ʾ�������֡��

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

	auto opos = static_cast<unsigned>((ipos_ + ilen) / factor_); // ȡfloor
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
