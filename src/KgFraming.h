#pragma once
#include "kDsp.h"
#include <memory>

class KcSampled1d;
class KcSampled2d;


// 对一维信号作分帧处理
class KgFraming
{
public:
	KgFraming(kReal sampleRate, kIndex channels = 1);

	void reset(kReal sampleRate, kIndex channels = 1);

	kReal shift() const { return shift_; }
	void setShift(kReal shift) { shift_ = shift; }

	kReal length() const { return length_; }
	void setLength(kReal len) { length_ = len; }


	// 输出的分帧结果可能有多个，所以用二维信号out表示
	void process(const KcSampled1d& in, KcSampled2d& out);

	void flush(KcSampled2d& out);

	// 给定采样点数目，计算可分帧数
	kIndex numFrames(kIndex samples);

	kIndex frameSize() const; // 每帧的采样点数目
	kIndex shiftSize() const; // 每次偏移的采样点数目

	// roundPower2_为false时，返回值等于frameSize，否则等于ceilPower2(frameSize)
	kIndex samplesPerFrame() const;

private:
	kReal shift_; // in second
	kReal length_; // in second

	// If true, round window size to power of two by zero-padding input to FFT.
	bool  roundPower2_; 

	std::unique_ptr<KcSampled1d> buf_; // 待处理数据，长度小于length_
};

