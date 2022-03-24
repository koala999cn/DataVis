#include "KgFraming.h"
#include "KcSampled1d.h"
#include "KcSampled2d.h"
#include "KtuBitwise.h"
#include "KtuMath.h"
#include <assert.h>


KgFraming::KgFraming(kReal sampleRate, kIndex channels)
    : roundPower2_(false)
{
	buf_ = std::make_unique<KcSampled1d>(1 / sampleRate, 0, channels);
	setLength(500 / sampleRate);
	setShift(250 / sampleRate);
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


void KgFraming::reset(kReal sampleRate, kIndex channels)
{
	buf_->reset(1 / sampleRate, channels);
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
	assert(buf_->count() < frameSize);
}


kIndex KgFraming::numFrames(kIndex samples)
{
	auto frameSize = this->frameSize();
	if (samples < frameSize || frameSize == 0)
		return 0;

	return shiftSize() == 0 ? 1 : 1 + (samples - frameSize) / shiftSize();
}
