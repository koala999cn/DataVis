#pragma once
#include "KcAudioDevice.h"
#include <memory>


class KcAudio;
class KgAudioFile;


// 自动挂接用于存储录音数据的observer
class KcAudioCapture : public KtObservable<void*, unsigned, double>
{
public:
    KcAudioCapture();


    // @deviceId: 录音设备id，-1表示选择默认录音设备
    // @sampleRate: 录音的采样频率，0表示选择录音设备的优选频率
    // @channels: 录制声道数
    // @frameTime: 录音帧的时长(sec), 每间隔该时长，录音设备会调用回调函数，若<=0，则使用上次录音设置，或使用缺省值
    bool record(unsigned deviceId = -1, unsigned sampleRate = 0, unsigned channels = 1, 
                double frameTime = 0, bool startImmediately = true);


    // @audio: 使用该参数的采样频率和通道数打开录音设备，并将录音结果保存到该参数
    bool record(std::shared_ptr<KcAudio>& audio, unsigned deviceId = -1, double frameTime = 0);


    // @file: 使用该参数的采样频率和通道数打开录音设备，并将录音结果保存到该参数
    bool record(std::shared_ptr<KgAudioFile>& file, unsigned deviceId = -1, double frameTime = 0);


    // 停止录制
    bool stop(bool wait);

    // 暂停录制
    bool pause(bool wait);

    // 继续录制
    bool goon();

    // 是否正在录制？
    bool running() const;

    // 是否暂停录制？
    bool pausing() const;


    unsigned channels() const { return device_->inputChannels(); }

    const char* errorText() const { return device_->errorText(); }

private:
    std::unique_ptr<KcAudioDevice> device_; // 录音设备
    unsigned openedDevice_;
};
