#pragma once
#include "KtFraming.h"
#include <cmath>


// ���ڲ�ֵ�㷨���ز���ʵ�ֿ��

// NOTE: length vs size
// length��ʾ������ĳ��ȣ���ͨ��������Ϊһ��
// size��ʾ���ݳ��ȣ�=length * channels()

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

	// �������ݣ��ݲ�ִ��resample����
	void push(const T* first, const T* last) { 
		framing_.push(first, last); 
	}

	void reset() { 
		framing_.reset(); 
		std::vector<T> buf(length() / 2 * channels(), 0);
		framing_.push(buf.data(), buf.data() + buf.size()); // ѹ��봰����
		obuf_.clear();
		ipos_ = 0, opos_ = 0;
	}


	template<typename INTERP> 
	unsigned apply(const T* in, unsigned ilen, T* out, unsigned olen, INTERP interp);

	template<typename INTERP> 
	unsigned flush(T* out, unsigned olen, INTERP interp);


private:

	// ��obuf_�е����������out������д��������鳤��
	unsigned flushBuf_(T* out, unsigned olen);

	// ������inlen�����ݣ������ɶ��������ݣ�
	unsigned inlenToOutlen_(unsigned insize) const;

	// Ҫ������������ݣ���������outlen�����ݣ�
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

				// ��ͨ����ֵ
				for (unsigned c = 0; c < chann; c++) 
					*obuf++ = interp(ibuf + c, len, chann, phase);
			}

			++ipos;
		}
	};

	friend template<typename INTERP> struct KpInterp_;

private:
	KtFraming<T> framing_;
	double factor_; // �ز���ϵ����= inrate/outrate������1��ʾ��������С��1��ʾ������
	unsigned ipos_; // �ۼ�������
	unsigned opos_; // �ۼ������
	std::vector<T> obuf_;  // �������
};


template<typename T> template<typename INTERP>
unsigned KtResampling<T>::apply(const T* in, unsigned ilen, T* out, unsigned olen, INTERP interp)
{
	auto chann = channels();

	// �������������
	auto oled = flushBuf_(out, olen); // ol��ʾ����������ݳ���

	// д��ʣ�µ�out
	auto ilen0 = outlenToInlen_(olen - oled); // ������Ҫ�����������д��out
	auto itpr = KpInterp_(*this, interp, out + oled, out + olen);
	framing_.apply(in, in + ilen0 * chann, itpr);
	oled = (itpr.obuf - out) / chann; // ���¼���д��out�������ܳ���

	// ���������룬�����obuf_
	auto ilen1 = ilen - ilen0;
	auto olen0 = inlenToOutlen_(ilen1);
	obuf_.resize(olen0 * chann);
	itpr.obuf = obuf_.data();
	itpr.oend = obuf_.data() + obuf_.size();
	framing_.apply(in + ilen0 * chann, in + ilen * chann, itpr);

	return oled; // ����д��out�����ݳ���
}


template<typename T> template<typename INTERP>
unsigned KtResampling<T>::flush(T* out, unsigned olen, INTERP interp)
{
	// �������������
	auto oled = flushBuf_(out, olen); // ol��ʾ����������ݳ���

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
	auto opos = static_cast<unsigned>((ipos_ + inlen) / factor_); // ȡfloor
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
