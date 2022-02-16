#include "KcAudioCapture.h"
#include "KcAudio.h"
#include "KgAudioFile.h"


namespace kPrivate
{
    // 主要实现3个observer:
    //   一是KcNotifyObserver，用来向KcAudioCapture的观察者转发update
    //   二是KcAudioCaptureObserver，用来将录制的音频数据保存到KcAudio
    //   三是KcFileCaptureObserver，用来将录制的音频数据保存到KgAudioFile

    class KcNofifyObserver : public KcAudioDevice::observer_type
    {
    public:

        KcNofifyObserver(KcAudioCapture& recorder) : recorder_(recorder){}

        bool update(void */*outputBuffer*/, void *inputBuffer,
                    unsigned frames, double streamTime) override {
            return recorder_.notify(inputBuffer, frames, streamTime);
        }

    private:
        KcAudioCapture& recorder_;
    };


    class KcAudioCaptureObserver : public KcAudioCapture::observer_type
    {
    public:

        KcAudioCaptureObserver(std::shared_ptr<KcAudio>& audio) : audio_(audio) {}

        bool update(void* inputBuffer, unsigned frames, double streamTime) override {
            audio_->addSamples((kReal*)inputBuffer, frames);
            return true;
        }

    private:
        std::shared_ptr<KcAudio> audio_; // 用来保存已录制的音频数据
    };


    class KcFileCaptureObserver : public KcAudioCapture::observer_type
    {
    public:

        KcFileCaptureObserver(std::shared_ptr<KgAudioFile>& file) : file_(file) {}

        bool update(void* inputBuffer, unsigned frames, double streamTime) override {
            return file_->write((kReal*)inputBuffer, frames) > 0;
        }

    private:
        std::shared_ptr<KgAudioFile> file_; // 用来保存已录制的音频数据
    };

}


using namespace kPrivate;

KcAudioCapture::KcAudioCapture()
{
    device_ = std::make_unique<KcAudioDevice>();
    device_->pushBack(std::make_shared<KcNofifyObserver>(*this));
    openedDevice_ = -1;
}


bool KcAudioCapture::record(unsigned deviceId, unsigned sampleRate, unsigned channels, 
                            double frameTime, bool startImmediately)
{
    assert(device_);

    if (deviceId == static_cast<unsigned>(-1))
        deviceId = device_->defaultInput();

    if (sampleRate == 0)
        sampleRate = device_->preferredSampleRate(deviceId);
    if (sampleRate == 0)
        return false;

    // 检测参数一致性，不一致则重新打开设备
    if (deviceId != openedDevice_ ||
        device_->inputChannels() != channels ||
        device_->sampleRate() != sampleRate ||
        (frameTime > 0 && frameTime != device_->frameDuration())) {

        if (device_->running()) device_->stop(true);
        if (device_->opened()) device_->close();
        if (frameTime <= 0) frameTime = 0.05; // 缺省帧长50ms

        KcAudioDevice::KpStreamParameters iParam;
        iParam.deviceId = deviceId;
        iParam.channels = channels;
        unsigned bufferFrames = unsigned(sampleRate * frameTime + 0.5);

        if (!device_->open(nullptr, &iParam,
            std::is_same<kReal, double>::value ? KcAudioDevice::k_float64 : KcAudioDevice::k_float32,
            sampleRate, bufferFrames))
            return false;

        openedDevice_ = deviceId;

        remove<KcAudioCaptureObserver>();
        remove<KcFileCaptureObserver>();
    }

    if (running()) stop(true);

    return startImmediately ? device_->start() : true;
}


bool KcAudioCapture::record(std::shared_ptr<KcAudio>& audio, unsigned deviceId, double frameTime)
{
    assert(audio);

    if (!record(deviceId, static_cast<unsigned>(audio->samplingRate()), 
        audio->channels(), frameTime, false))
        return false;

    assert(get<KcAudioCaptureObserver>() == nullptr);
    assert(get<KcFileCaptureObserver>() == nullptr);
    
    pushBack(std::make_shared<KcAudioCaptureObserver>(audio));

    device_->setStreamTime(audio->xrange().first);
    return device_->start();
}


bool KcAudioCapture::record(std::shared_ptr<KgAudioFile>& file, unsigned deviceId, double frameTime)
{
    assert(file);

    if (!record(deviceId, file->sampleRate(), file->channels(), frameTime, false))
        return false;

    assert(get<KcAudioCaptureObserver>() == nullptr);
    assert(get<KcFileCaptureObserver>() == nullptr);

    pushBack(std::make_shared<KcFileCaptureObserver>(file));

    return device_->start();
}


bool KcAudioCapture::stop(bool wait)
{ 
    assert(running() || pausing());

    if (running() && !device_->stop(wait))
        return false;

    assert(!running());
 
    openedDevice_ = -1;

    
    remove<KcAudioCaptureObserver>();
    remove<KcFileCaptureObserver>();

    return true;
}


bool KcAudioCapture::pause(bool wait)
{
    assert(running());
    return device_->stop(wait);
}


bool KcAudioCapture::goon() 
{ 
    assert(pausing());
    return device_->start(); 
}


bool KcAudioCapture::running() const
{
    return device_->running();
}


bool KcAudioCapture::pausing() const
{
    return !running() && openedDevice_ != -1;
}