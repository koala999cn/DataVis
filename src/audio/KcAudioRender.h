#pragma once
#include <memory>
#include "KcAudioDevice.h"

class KcSampled1d;
class KgAudioFile;

class KcAudioRender : public KtObservable<void*, unsigned, double>
{
public:
    KcAudioRender();
    ~KcAudioRender();

    // 将数据插入缓存队列
    void enqueue(const std::shared_ptr<KcSampled1d>& data);

    // 清空缓存队列
    void reset(); 

    // 播放缓存队列
    // assert(!queue_->empty())
    // 自动匹配换队队列采样频率和通道数
    // 保持缓存队列状态
    bool play(unsigned deviceId = -1, double frameTime = 0);

    // 使用指定设备和频率、声道参数播放缓存队列
    // 若设备不支持给定的参数，返回false
    bool play(unsigned deviceId, unsigned sampleRate, unsigned channels, double frameTime = 0);


    // 播放data对象
    // 自动匹配data采样频率和通道数
    // 开始播放前，清空缓存队列
    bool play(const std::shared_ptr<KcSampled1d>& data, unsigned deviceId = -1, double frameTime = 0);

    // 播放filePath指向路径的音频文件
    // 自动匹配file采样频率和通道数
    // 开始播放前，清空缓存队列
    //bool play(const std::shared_ptr<KgAudioFile>& file, unsigned deviceId = -1, double frameTime = 0);

    bool stop(bool wait);

    bool pause(bool wait);

    bool goon();

     // 是否正在播放
    bool running() const;

    bool opened() const;


    // 是否暂停播放
    bool paused() const;

    bool stopped() const;


    const char* errorText() const;

    unsigned defaultDevice() const;

    unsigned preferredSampleRate(unsigned deviceId) const;

    bool autoStop() const { return autoStop_; }
    void setAutoStop(bool b) { autoStop_ = b; }

private:

    // open specific output audio 
    // @deviceId, 播放设备id，-1表示选取系统默认音频输出设备
    // @sampleRate, 采样频率，若设备不支持，返回false
    // @channels, 声道数，若设备不支持，返回false
    // @frameTime, 每次播放回调搜集的音频时长
    bool openDevice_(unsigned deviceId, unsigned sampleRate, unsigned channels, double frameTime = 0);

    // 若deviceId为有效的输入设备，则该函数用deviceId设备支持的频率和声道数尝试打开操作
    bool openBestMatch_(unsigned deviceId, unsigned& sampleRate, unsigned& channels, double frameTime = 0);

    bool closeDevice_();

private:
    std::unique_ptr<KcAudioDevice> device_; // 播放设备
    unsigned openedDevice_;
    void* queue_; // 缓存队列，暂存待播放音频数据
    bool autoStop_{ true }; // 当数据耗尽时，是否自动停止设备？
};
