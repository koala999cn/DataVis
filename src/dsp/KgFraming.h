#pragma once
#include <vector>
#include <assert.h>
#include "kDsp.h"


// ���ŵ����ݵķ�֡������

class KgFraming
{
public:

	constexpr static bool isInterleaving() { return true; }

	// �ӳ�len�����ݵ����
	// @shift: =0ʱ��ȡlenֵ
	KgFraming(unsigned len, unsigned channels, unsigned shift)
		: leng_(len), chann_(channels), buf_(), shift_(shift ? shift : len) {}

	// ��ʼ���ڲ�״̬ΪinitVal������ʱ���
	KgFraming(unsigned len, unsigned channels, unsigned shift, kReal initVal)
	: leng_(len), chann_(channels), buf_(len, initVal), shift_(shift ? shift : len) {}

	unsigned length() const { return leng_; }
	unsigned shift() const { return shift_; }
	unsigned channels() const { return chann_; }
	unsigned buffered() const { return buf_.size() / channels(); }

	// framing in place
	template<typename OP>
	void apply(const kReal* first, const kReal* last, OP op) {
		auto incount = (last - first) / channels(); // �����������������
		if (numFrames(incount, true) == 0) { // �����������㣬��ִ�л������
			push_(first, last);
			return;
		}

		// ������ִ�з�֡��
		//   һ�ǿ����������뵽���棬�Ⱥľ��������ݣ�
		//   ���Ƕ�ʣ�������ִ�����ߴ���
		auto copycount = needAppended_();
		assert(numFrames(copycount, true) * shift_ >= buffered());
		if (copycount > incount)
			copycount = incount;
		auto last_ = first + copycount * channels();
		push_(first, last_);
		auto pos = execute_(buf_.data(), buf_.data() + buf_.size(), op); // ִ�е�һ�׶η�֡

		if (last_ == last) { // ��ʣ������
			buf_.erase(buf_.begin(), buf_.begin() + (pos - buf_.data()));
			assert(numFrames(0, true) == 0);
		}
		else { // ����ʣ������
			first = last_ - (buf_.data() + buf_.size() - pos);
			pos = execute_(first, last, op); // ִ�еڶ��׶η�֡
			buf_.clear();
			push_(pos, last); // ��������ѹ�뻺��
		}
	}

	
	template<typename OP>
	void flush(OP op) {
		if (!buf_.empty()) {
			buf_.resize(length() * channels(), 0);
			op(buf_.data());
			buf_.clear();
		}
	}


	// @addBuffered: ��Ϊtrue������samples�����ϼ��뻺�����ݽ���֡������
	unsigned numFrames(unsigned samples, bool addBuffered) const {
		if (addBuffered)
			samples += buffered();

		// ����shift > length���������֤������ִ��һ��shift����ʱ�ٷ�֡
		if (samples < std::max(length(), shift())) 
			return 0;

		assert(shift() != 0);
		return (samples - length()) / shift() + 1;
	}


private:

	// ������ѹ�뻺��
	void push_(const kReal* first, const kReal* last) {
		buf_.insert(buf_.end(), first, last);
	}

	// ������Ҫ�򻺴濽���������ݣ����ܷ�֡�ľ�����
	unsigned needAppended_() const {
		if (buf_.empty())
			return 0;
		
		return ((buffered() - 1) / shift_) * shift_ + leng_ - buffered();
	}

	// ������������ִ�з�֡�������ǻ������ݣ����ز������ݵ���ָ��
	template<typename OP>
	const kReal* execute_(const kReal* first, const kReal* last, OP op) {
		auto incount = (last - first) / channels();
		auto frames = numFrames(incount, false);
		auto shiftRaw = shift_ * channels();
		for (unsigned i = 0; i < frames; i++, first += shiftRaw)
			op(first);

		return first;
	}


private:
	std::vector<kReal> buf_;
	unsigned chann_; // �ŵ���
	unsigned leng_; // ֡��
	unsigned shift_; // ֡��
};

