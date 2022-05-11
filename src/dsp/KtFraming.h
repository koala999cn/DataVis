#pragma once
#include <vector>
#include <assert.h>


// 对信道数据的分帧处理器

template<typename T>
class KtFraming
{
public:

	constexpr static bool isInterleaving() { return true; }

	// 延迟len个数据点输出
	// @shift: =0时，取len值
	KtFraming(unsigned len, unsigned chann, unsigned shift)
		: length_(len), chann_(chann), buf_(), shift_(shift ? shift : len) {}

	// 初始化内部状态为initVal，无延时输出
	KtFraming(unsigned len, unsigned chann, unsigned shift, T initVal)
	: length_(len), chann_(chann), buf_((len - 1) * chann, initVal), shift_(shift ? shift : len) {}

	unsigned length() const { return length_; }
	unsigned shift() const { return shift_; }
	unsigned channels() const { return chann_; }
	unsigned buffered() const { return buf_.size() / channels(); }

	// framing in place
	template<typename OP> void apply(const T* first, const T* last, OP op);

	template<typename OP> void flush(OP op);

	// 将数据压入缓存，延迟执行op操作
	void push(const T* first, const T* last) {
		buf_.insert(buf_.end(), first, last);
	}

	// 清空缓存
	void reset() {
		buf_.clear();
	}

	// 重置
	void reset(unsigned len, unsigned chann, unsigned shift) {
		length_ = len, chann_ = chann, shift_ = shift;
	    buf_.clear();
	}

	// @addBuffered: 若为true，则在samples基础上加入缓存数据进行帧数计算
	unsigned numFrames(unsigned samples, bool addBuffered) const;


private:

	// 计算需要向缓存拷贝多少数据，才能分帧耗尽缓存
	unsigned needAppended_() const;

	// 在连续数据上执行分帧，不考虑缓存数据，返回残留数据迭代指针
	template<typename OP>
	const T* execute_(const T* first, const T* last, OP& op);


private:
	std::vector<T> buf_;
	unsigned chann_; // 信道数
	unsigned length_; // 帧长
	unsigned shift_; // 帧移
};


template<typename T> template<typename OP>
void KtFraming<T>::apply(const T* first, const T* last, OP op)
{
	auto incount = (last - first) / channels(); // 新增输入的数据数量
	if (numFrames(incount, true) == 0) { // 若数据量不足，仅执行缓存操作
		push(first, last);
		return;
	}

	// 分两段执行分帧：
	//   一是拷贝部分输入到缓存，先耗尽缓存数据，
	//   二是对剩余的输入执行在线处理
	auto copycount = needAppended_();
	assert(numFrames(copycount, true) * shift_ >= buffered());
	if (copycount > incount)
		copycount = incount;
	auto last_ = first + copycount * channels();
	push(first, last_);
	auto pos = execute_(buf_.data(), buf_.data() + buf_.size(), op); // 执行第一阶段分帧

	if (last_ == last) { // 无剩余数据
		buf_.erase(buf_.begin(), buf_.begin() + (pos - buf_.data()));
		assert(numFrames(0, true) == 0);
	}
	else { // 处理剩余数据
		first = last_ - (buf_.data() + buf_.size() - pos);
		pos = execute_(first, last, op); // 执行第二阶段分帧
		buf_.clear();
		push(pos, last); // 残留数据压入缓存
	}
}


template<typename T> template<typename OP>
void KtFraming<T>::flush(OP op)
{
	if (!buf_.empty()) {
		buf_.resize(length() * channels(), 0); // TODO: 处理buf_尺寸大于length_的情况
		op(buf_.data());
		buf_.clear();
	}
}


template<typename T>
unsigned KtFraming<T>::numFrames(unsigned samples, bool addBuffered) const
{
	if (addBuffered)
		samples += buffered();

	// 考虑shift > length的情况，保证最少能执行一次shift操作时再分帧
	if (samples < std::max(length(), shift()))
		return 0;

	assert(shift() != 0);
	return (samples - length()) / shift() + 1;
}


template<typename T> template<typename OP>
const T* KtFraming<T>::execute_(const T* first, const T* last, OP& op) 
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

