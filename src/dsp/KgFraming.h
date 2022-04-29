#pragma once
#include <vector>
#include <assert.h>
#include "kDsp.h"


// 对信道数据的分帧处理器

class KgFraming
{
public:

	constexpr static bool isInterleaving() { return true; }

	// 延迟len个数据点输出
	// @shift: =0时，取len值
	KgFraming(unsigned len, unsigned channels, unsigned shift)
		: leng_(len), chann_(channels), buf_(), shift_(shift ? shift : len) {}

	// 初始化内部状态为initVal，无延时输出
	KgFraming(unsigned len, unsigned channels, unsigned shift, kReal initVal)
	: leng_(len), chann_(channels), buf_(len, initVal), shift_(shift ? shift : len) {}

	unsigned length() const { return leng_; }
	unsigned shift() const { return shift_; }
	unsigned channels() const { return chann_; }
	unsigned buffered() const { return buf_.size() / channels(); }

	// framing in place
	template<typename OP>
	void apply(const kReal* first, const kReal* last, OP op) {
		auto incount = (last - first) / channels(); // 新增输入的数据数量
		if (numFrames(incount, true) == 0) { // 若数据量不足，仅执行缓存操作
			push_(first, last);
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
		push_(first, last_);
		auto pos = execute_(buf_.data(), buf_.data() + buf_.size(), op); // 执行第一阶段分帧

		if (last_ == last) { // 无剩余数据
			buf_.erase(buf_.begin(), buf_.begin() + (pos - buf_.data()));
			assert(numFrames(0, true) == 0);
		}
		else { // 处理剩余数据
			first = last_ - (buf_.data() + buf_.size() - pos);
			pos = execute_(first, last, op); // 执行第二阶段分帧
			buf_.clear();
			push_(pos, last); // 残留数据压入缓存
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


	// @addBuffered: 若为true，则在samples基础上加入缓存数据进行帧数计算
	unsigned numFrames(unsigned samples, bool addBuffered) const {
		if (addBuffered)
			samples += buffered();

		// 考虑shift > length的情况，保证最少能执行一次shift操作时再分帧
		if (samples < std::max(length(), shift())) 
			return 0;

		assert(shift() != 0);
		return (samples - length()) / shift() + 1;
	}


private:

	// 将数据压入缓存
	void push_(const kReal* first, const kReal* last) {
		buf_.insert(buf_.end(), first, last);
	}

	// 计算需要向缓存拷贝多少数据，才能分帧耗尽缓存
	unsigned needAppended_() const {
		if (buf_.empty())
			return 0;
		
		return ((buffered() - 1) / shift_) * shift_ + leng_ - buffered();
	}

	// 在连续数据上执行分帧，不考虑缓存数据，返回残留数据迭代指针
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
	unsigned chann_; // 信道数
	unsigned leng_; // 帧长
	unsigned shift_; // 帧移
};

