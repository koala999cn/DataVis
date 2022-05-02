#pragma once
#include <vector>
#include <assert.h>


// ���ŵ����ݵķ�֡������

template<typename T>
class KtFraming
{
public:

	constexpr static bool isInterleaving() { return true; }

	// �ӳ�len�����ݵ����
	// @shift: =0ʱ��ȡlenֵ
	KtFraming(unsigned len, unsigned chann, unsigned shift)
		: length_(len), chann_(chann), buf_(), shift_(shift ? shift : len) {}

	// ��ʼ���ڲ�״̬ΪinitVal������ʱ���
	KtFraming(unsigned len, unsigned chann, unsigned shift, T initVal)
	: length_(len), chann_(chann), buf_((len - 1) * chann, initVal), shift_(shift ? shift : len) {}

	unsigned length() const { return length_; }
	unsigned shift() const { return shift_; }
	unsigned channels() const { return chann_; }
	unsigned buffered() const { return buf_.size() / channels(); }

	// framing in place
	template<typename OP> void apply(const T* first, const T* last, OP op);

	template<typename OP> void flush(OP op);

	// @addBuffered: ��Ϊtrue������samples�����ϼ��뻺�����ݽ���֡������
	unsigned numFrames(unsigned samples, bool addBuffered) const;


private:

	// ������ѹ�뻺��
	void push_(const T* first, const T* last) {
		buf_.insert(buf_.end(), first, last);
	}

	// ������Ҫ�򻺴濽���������ݣ����ܷ�֡�ľ�����
	unsigned needAppended_() const;

	// ������������ִ�з�֡�������ǻ������ݣ����ز������ݵ���ָ��
	template<typename OP>
	const T* execute_(const T* first, const T* last, OP op);


private:
	std::vector<T> buf_;
	unsigned chann_; // �ŵ���
	unsigned length_; // ֡��
	unsigned shift_; // ֡��
};


template<typename T> template<typename OP>
void KtFraming<T>::apply(const T* first, const T* last, OP op)
{
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


template<typename T> template<typename OP>
void KtFraming<T>::flush(OP op)
{
	if (!buf_.empty()) {
		buf_.resize(length() * channels(), 0);
		op(buf_.data());
		buf_.clear();
	}
}


template<typename T>
unsigned KtFraming<T>::numFrames(unsigned samples, bool addBuffered) const
{
	if (addBuffered)
		samples += buffered();

	// ����shift > length���������֤������ִ��һ��shift����ʱ�ٷ�֡
	if (samples < std::max(length(), shift()))
		return 0;

	assert(shift() != 0);
	return (samples - length()) / shift() + 1;
}


template<typename T> template<typename OP>
const T* KtFraming<T>::execute_(const T* first, const T* last, OP op) 
{
	auto incount = (last - first) / channels();
	auto frames = numFrames(incount, false);
	auto shiftRaw = shift_ * channels();
	for (unsigned i = 0; i < frames; i++, first += shiftRaw)
		op(first);

	return first;
}


template<typename T>
unsigned KtFraming<T>::needAppended_() const 
{
	if (buf_.empty())
		return 0;

	return ((buffered() - 1) / shift_) * shift_ + length_ - buffered();
}

