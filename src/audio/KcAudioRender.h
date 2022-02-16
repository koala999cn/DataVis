#pragma once
#include <memory>
#include "KcAudioDevice.h"

class KcAudio;
class KgAudioFile;

class KcAudioRender : public KtObservable<void*, unsigned, double>
{
public:
    KcAudioRender();

    // 播放audio对象
    // @deviceId, 播放设备id，-1表示选取系统默认音频输出设备
    bool playback(const std::shared_ptr<KcAudio>& audio, unsigned deviceId = -1, double frameTime = 0);


    // 播放filePath指向路径的音频文件
    bool playback(const std::shared_ptr<KgAudioFile>& file, unsigned deviceId = -1, double frameTime = 0);


    bool stop(bool wait);

    bool pause(bool wait);

    bool goon(bool wait);

    // 是否正在播放
    bool running() const;

    // 是否暂停播放
    bool pausing() const;


    const char* errorText() const { return device_->errorText(); }


protected:

    // open specific output audio device
    bool open_(unsigned deviceId, unsigned sampleRate, unsigned channels, double frameTime = 0);


private:
    std::unique_ptr<KcAudioDevice> device_; // 播放设备
    unsigned openedDevice_;
};
