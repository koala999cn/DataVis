#include "KcAudioRender.h"
#include "KcAudio.h"
#include "KgAudioFile.h"
#include <string.h>


namespace kPrivate
{
    // 主要实现3个observer:
    //   一是KcNotifyObserver，用来向KcAudioRender的观察者转发update
    //   二是KcAudioRenderObserver，用来回放KcAudio音频数据
    //   三是KcFileRenderObserver，用来回放KgAudioFile音频文件

    class KcNotifyObserver : public KcAudioDevice::observer_type
    {
    public:
        KcNotifyObserver(KcAudioRender& render) : render_(render) {}

        bool update(void* outputBuffer, void* /*inputBuffer*/,
                    unsigned frames, double streamTime) override {
            return render_.notify(outputBuffer, frames, streamTime);
        }

    private:
        KcAudioRender& render_;
    };


    class KcAudioRenderObserver : public KcAudioRender::observer_type
    {
    public:
        KcAudioRenderObserver(std::shared_ptr<KcAudio> audio) : audio_(audio) {}

        bool update(void* outputBuffer, unsigned frames, double streamTime) override {

            auto buf = (kReal*)outputBuffer;
            auto pos = audio_->sampling().xToHighIndex(streamTime);
            if (pos > audio_->count())
                pos = audio_->count();

            kIndex total = std::min<kIndex>(frames, audio_->count() - pos);
            audio_->getSamples(pos, buf, total);
            ::memset(buf + audio_->channels() * total, 0, audio_->bytesOfSamples(frames - total));

            pos += total;

            return pos < audio_->count();
        }

    private:
        std::shared_ptr<KcAudio> audio_;
    };


    class KcFileRenderObserver : public KcAudioRender::observer_type
    {
    public:
        KcFileRenderObserver(std::shared_ptr<KgAudioFile> file) : file_(file) {}

        bool update(void* outputBuffer, unsigned frames, double streamTime) override {

            auto buf = (kReal*)outputBuffer;
            auto read = file_->read(buf, frames);
            ::memset(buf + read * file_->channels(), 0, file_->channels() * (frames - read) * sizeof(kReal));

            if (read == 0) {
                file_->close();
                return false;
            }

            return true;
        }

    private:
        std::shared_ptr<KgAudioFile> file_;
    };
}


KcAudioRender::KcAudioRender()
{
    device_ = std::make_unique<KcAudioDevice>();
    device_->pushBack(std::make_shared<kPrivate::KcNotifyObserver>(*this));
    openedDevice_ = -1;
}


bool KcAudioRender::playback(const std::shared_ptr<KcAudio>& audio, unsigned deviceId, double frameTime)
{
    assert(audio);
  
    if (!open_(deviceId, audio->samplingRate(), audio->channels(), frameTime))
        return false;

    assert(get<kPrivate::KcAudioRenderObserver>() == nullptr);
    assert(get<kPrivate::KcFileRenderObserver>() == nullptr);
    pushFront(std::make_shared<kPrivate::KcAudioRenderObserver>(audio)); // 放在最前面，这样写入的数据才能被其他观察者看到

    device_->setStreamTime(audio->xrange().first);
    return device_->start();
}


bool KcAudioRender::playback(const std::shared_ptr<KgAudioFile>& file, unsigned deviceId, double frameTime)
{
    assert(file);

    if (!open_(deviceId, file->sampleRate(), file->channels(), frameTime))
        return false;

    assert(get<kPrivate::KcAudioRenderObserver>() == nullptr);
    assert(get<kPrivate::KcFileRenderObserver>() == nullptr);
    pushFront(std::make_shared<kPrivate::KcFileRenderObserver>(file)); // 放在最前面，这样写入的数据才能被其他观察者看到

    return device_->start();
}


bool KcAudioRender::stop(bool wait)
{
    if (running() && !device_->stop(wait))
        return false;

    remove<kPrivate::KcAudioRenderObserver>();
    remove<kPrivate::KcFileRenderObserver>();

    return true;
}


bool KcAudioRender::pause(bool wait)
{
    assert(running());
    return device_->stop(wait);
}


bool KcAudioRender::goon(bool wait)
{
    assert(pausing());
    return device_->start();
}


bool KcAudioRender::running() const
{
    return device_->running();
}


bool KcAudioRender::pausing() const
{
    return !running() && openedDevice_ != -1;
}


bool KcAudioRender::open_(unsigned deviceId, unsigned sampleRate, unsigned channels, double frameTime)
{
    assert(device_);

    if (deviceId == static_cast<unsigned>(-1))
        deviceId = device_->defaultOutput();

    // 检测参数一致性，不一致则重新打开设备
    if (deviceId != openedDevice_ ||
        device_->outputChannels() != channels ||
        device_->sampleRate() != sampleRate ||
        (frameTime > 0 && frameTime != device_->frameDuration())) {

        if (device_->opened()) device_->close();
        if (frameTime <= 0) frameTime = 0.05; // 缺省帧长50ms

        KcAudioDevice::KpStreamParameters oParam;
        oParam.deviceId = deviceId;
        oParam.channels = channels;
        unsigned bufferFrames = unsigned(sampleRate * frameTime + 0.5);

        if (!device_->open(&oParam, nullptr,
            std::is_same<kReal, double>::value ? KcAudioDevice::k_float64 : KcAudioDevice::k_float32,
            static_cast<unsigned>(sampleRate), bufferFrames))
            return false;

        openedDevice_ = deviceId;

        remove<kPrivate::KcAudioRenderObserver>();
        remove<kPrivate::KcFileRenderObserver>();
    }
    else if (running()) {
        if (!stop(true))
            return false;
    }

    return true;
}