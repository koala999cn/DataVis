#include "KcAudioRender.h"
#include "KvSampled.h"
#include "KgAudioFile.h"
#include "readerwriterqueue/readerwriterqueue.h"
#include <string.h>
#include "KtuMath.h"


namespace kPrivate
{
    using data_queue = moodycamel::ReaderWriterQueue<std::shared_ptr<KvSampled>>;

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
        KcAudioRenderObserver(data_queue* q, unsigned channels, bool& autoStop)
            : q_(q), channels_(channels), autoStop_(autoStop), pos_(0), data_() {}

        bool update(void* outputBuffer, unsigned frames, double streamTime) override {

            auto buf = (kReal*)outputBuffer;

            if (data_ == nullptr) {
                if (!q_->try_dequeue(data_)) {
                    KtuMath<kReal>::zeros(buf, frames * channels_);
                    return !autoStop_;
                }

                pos_ = 0;
            }

            assert(data_ && data_->channels() == channels_); // TODO: 频率也是一致的

            assert(data_->size(0) > pos_);
            auto toCopy = data_->size(0) - pos_;
            if (toCopy > frames) toCopy = frames;

            // data_->extract(pos_, buf, toCopy);
            // pos_ += toCopy;
            for (unsigned i = 0; i < toCopy; i++, pos_++)
                for (kIndex ch = 0; ch < channels_; ch++)
                    *buf++ = data_->value(pos_, ch);           
            
            auto dx = data_->step(0); // 先获取data的dx，下一步data可能被重置
            if (pos_ == data_->size())
                data_.reset(); // data_已耗尽

            if (toCopy < frames)
                return update(buf , frames - toCopy, streamTime + dx * toCopy);

            return true;
        }

    private:
        unsigned channels_;
        data_queue* q_;
        std::shared_ptr<KvSampled> data_;
        unsigned pos_;
        bool& autoStop_;
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

    queue_ = new typename kPrivate::data_queue;
}


KcAudioRender::~KcAudioRender()
{
    delete static_cast<kPrivate::data_queue*>(queue_);
}


bool KcAudioRender::play(const std::shared_ptr<KvSampled>& data, unsigned deviceId, double frameTime)
{
    assert(data);

    if (!stop(true))
        return false;

    reset();
    enqueue(data);
    return play(deviceId, frameTime);
}

/*
bool KcAudioRender::play(const std::shared_ptr<KgAudioFile>& file, unsigned deviceId, double frameTime)
{
    assert(file);

    if (!open(deviceId, file->sampleRate(), file->channels(), frameTime))
        return false;

    assert(get<kPrivate::KcAudioRenderObserver>() == nullptr);
    assert(get<kPrivate::KcFileRenderObserver>() == nullptr);
    pushFront(std::make_shared<kPrivate::KcFileRenderObserver>(file)); // 放在最前面，这样写入的数据才能被其他观察者看到

    return device_->start();
}*/


void KcAudioRender::enqueue(const std::shared_ptr<KvSampled>& data)
{
    ((kPrivate::data_queue*)queue_)->enqueue(data);
}


void KcAudioRender::reset()
{
    while(((kPrivate::data_queue*)queue_)->pop());
}


bool KcAudioRender::play(unsigned deviceId, double frameTime)
{
    auto data = ((kPrivate::data_queue*)queue_)->peek();
    if (data == nullptr)
        return false;

    auto samp = data->get();
    unsigned rate = static_cast<unsigned>(1. / samp->step(0));
    unsigned chan = samp->channels();
    if (!openBestMatch_(deviceId, rate, chan, frameTime))
        return false;

    assert(get<kPrivate::KcAudioRenderObserver>() == nullptr);
    assert(get<kPrivate::KcFileRenderObserver>() == nullptr);
    pushFront(std::make_shared<kPrivate::KcAudioRenderObserver>(
        (kPrivate::data_queue*)queue_, chan, autoStop_)); // 放在最前面，这样写入的数据才能被其他观察者看到

    device_->setStreamTime(samp->range(0).low());
    return device_->start();
}


bool KcAudioRender::play(unsigned deviceId, unsigned sampleRate, unsigned channels, double frameTime)
{
    if (!openDevice_(deviceId, sampleRate, channels, frameTime))
        return false;

    assert(get<kPrivate::KcAudioRenderObserver>() == nullptr);
    assert(get<kPrivate::KcFileRenderObserver>() == nullptr);
    pushFront(std::make_shared<kPrivate::KcAudioRenderObserver>(
        (kPrivate::data_queue*)queue_, channels, autoStop_));

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


bool KcAudioRender::goon()
{
    assert(paused());
    return device_->start();
}


bool KcAudioRender::closeDevice_()
{
    if (!device_->close())
        return false;
    openedDevice_ = -1;
    return true;
}


bool KcAudioRender::running() const
{
    return device_->running();
}


bool KcAudioRender::opened() const
{
    return device_->opened();
}


bool KcAudioRender::paused() const
{
    return !running() &&
        const_cast<KcAudioRender*>(this)->get<kPrivate::KcAudioRenderObserver>();
}


bool KcAudioRender::stopped() const
{
    return !running() && 
        const_cast<KcAudioRender*>(this)->get<kPrivate::KcAudioRenderObserver>() == nullptr;
}


bool KcAudioRender::openDevice_(unsigned deviceId, unsigned sampleRate, unsigned channels, double frameTime)
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


bool KcAudioRender::openBestMatch_(unsigned deviceId, unsigned& sampleRate, unsigned& channels, double frameTime)
{
    if (deviceId == static_cast<unsigned>(-1))
        deviceId = device_->defaultOutput();

    auto di = device_->info(deviceId);
    if (channels > di.outputChannels)
        channels = di.outputChannels;
    sampleRate = device_->bestMatch(deviceId, sampleRate, 1); // 优选升采样

    return openDevice_(deviceId, sampleRate, channels, frameTime);
}


const char* KcAudioRender::errorText() const
{ 
    return device_->errorText(); 
}


unsigned KcAudioRender::defaultDevice() const
{
    return device_->defaultOutput();
}


unsigned KcAudioRender::preferredSampleRate(unsigned deviceId) const
{
    return device_->preferredSampleRate(deviceId);
}
