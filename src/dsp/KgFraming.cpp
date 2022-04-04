#include "KgFraming.h"
#include "KcSampled1d.h"
#include "KcSampled2d.h"
#include "KtuBitwise.h"
#include "KtuMath.h"
#include <assert.h>


KgFraming::KgFraming(kReal dt, kIndex channels)
    : roundPower2_(false)
{
	buf_ = std::make_unique<KcSampled1d>(dt, 0, channels);
	setLength(512 * dt); // 默认帧长512个采样点
	setShift(256 * dt); // 默认帧移256个采样点
}


void KgFraming::setShift(kReal shift)
{
	shift_ = shift;
	shiftSize_ = buf_->sampling().countLength(shift);
}


void KgFraming::setLength(kReal len)
{
	length_ = len;
	frameSize_ = buf_->sampling().countLength(len);
	samplesPerFrame_ = roundPower2_ ? KtuBitwise<kIndex>::ceilPower2(frameSize_) : frameSize_;
}


void KgFraming::reset(kReal dt, kIndex channels)
{
	buf_->reset(dt, channels);
	setShift(shift_);
	setLength(length_);
}


void KgFraming::flush(KcSampled2d& out)
{
	out.clear();

	if (!buf_->empty()) {

		auto samplesPerFrame = this->samplesPerFrame();
		out.resize(1, samplesPerFrame, buf_->channels());
		out.reset(0, buf_->sampling().low() + length_ / 2, shift_);
		out.reset(1, buf_->sampling().low(), buf_->step(0));
		
		kReal* buf = out.at(0);
		buf_->getSamples(0, buf, buf_->count());
		::memset(buf + buf_->count(), 0, buf_->bytesOfSamples(samplesPerFrame - buf_->count()));
	}
}


void KgFraming::process(const KcSampled1d& in, KcSampled2d& out)
{
    assert(KtuMath<kReal>::almostEqualRel(buf_->samplingRate(), in.samplingRate()));
	assert(in.channels() == buf_->channels());

    // 计算输入信号能分多少帧
	buf_->append(in);
	auto frames = numFrames(buf_->count());

    // 预留输出空间
	auto frameSize = this->frameSize();
	auto samplesPerFrame = this->samplesPerFrame();
	out.resize(frames, samplesPerFrame, buf_->channels());
	out.reset(0, buf_->sampling().low() + length_ / 2, shift_);
	out.reset(1, buf_->sampling().low(), buf_->step(0));


	// 输出分帧
	auto shiftSize = this->shiftSize();
	kIndex frameFirst(0);
	for (kIndex i = 0; i < frames; i++) {
		assert(frameFirst + frameSize <= buf_->count());
		kReal* buf = out.at(i);
		buf_->getSamples(frameFirst, buf, frameSize);

		if (samplesPerFrame != frameSize)  // 补零
			::memset(buf + frameSize, 0, sizeof(kReal) * (samplesPerFrame - frameSize));

		frameFirst += shiftSize;
	}

	// 修正缓存
	buf_->popFront(frameFirst);
	assert(buf_->count() < frameSize + shiftSize);
}


kIndex KgFraming::numFrames(kIndex samples)
{
	if (samples < frameSize() || frameSize() == 0
		|| samples < shiftSize()) // 考虑shift > length的情况，保证最少能执行一次shift操作时再分帧
		return 0;

	assert(shiftSize() != 0);
	return (samples - frameSize()) / shiftSize();
}
