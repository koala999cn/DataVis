#include "KcAudioInputStream.h"
#include "audio/KcAudio.h"
#include "audio/KcAudioDevice.h"
#include "KtSampling.h"


namespace kPrivate
{
    class KcAudioStreamObserver : public KcAudioDevice::observer_type
    {
    public:

        KcAudioStreamObserver(KcAudioInputStream* ais) {
            stream_ = ais;
            dx_ = kReal(1) / ais->sampleRate();
            channles_ = ais->channels();
        }

        bool update(void*/*outputBuffer*/, void* inputBuffer, unsigned frames, double streamTime) override {
            auto data = std::make_shared<KcAudio>(dx_, 0, channles_);
            data->addSamples(static_cast<kReal*>(inputBuffer), frames);
            //data->sampling().shift(streamTime);
            emit stream_->onData(data);
            return true;
        }

    private:
        KcAudioInputStream* stream_; 
        kReal dx_;
        int channles_;
    };


	enum KeAudioInputPropertyId
	{
		k_device_id,
		k_channels,
		k_sample_rate,
		k_frame_time,
	};
}


KcAudioInputStream::KcAudioInputStream() 
    : KvDataStream("AudioInput")
{
    dptr_ = new KcAudioDevice;
    deviceId_ = ((KcAudioDevice*)dptr_)->defaultInput();
    channels_ = 1;
    sampleRate_ = ((KcAudioDevice*)dptr_)->preferredSampleRate(deviceId_);
    frameTime_ = 0.1;
}


KcAudioInputStream::~KcAudioInputStream()
{
    delete (KcAudioDevice*)dptr_;
}


bool KcAudioInputStream::pushData()
{
    auto device = (KcAudioDevice*)dptr_;
    if (device->opened())
        device->close();

    device->remove<kPrivate::KcAudioStreamObserver>(); // 根据当前参数重新添加观察者

    KcAudioDevice::KpStreamParameters iParam;
    iParam.deviceId = deviceId_;
    iParam.channels = channels_;

	KtSampling<kReal> samp(kReal(0), kReal(frameTime_), kReal(1) / sampleRate_, 0);
	unsigned bufferFrames = samp.count(); // unsigned(sampleRate_ * frameTime_ + 0.5);

    if (!device->open(nullptr, &iParam,
        std::is_same<kReal, double>::value ? KcAudioDevice::k_float64 : KcAudioDevice::k_float32,
        sampleRate_, bufferFrames))
        return false;

    device->pushBack(std::make_shared<kPrivate::KcAudioStreamObserver>(this));
    return device->start();
}


void KcAudioInputStream::stop()
{
    ((KcAudioDevice*)dptr_)->stop(true);
    ((KcAudioDevice*)dptr_)->close();

}


bool KcAudioInputStream::running() const
{
	return ((KcAudioDevice*)dptr_)->running();
}


kRange KcAudioInputStream::range(kIndex axis) const
{
	return axis == 0 ? kRange{ 0, frameTime_ } : kRange{ -1, 1 };
}


kReal KcAudioInputStream::step(kIndex axis) const
{
	return axis == 0 ? static_cast<kReal>(1) / sampleRate() : KvData::k_nonuniform_step;
}


KcAudioInputStream::kPropertySet KcAudioInputStream::propertySet() const
{
	KvPropertiedObject::kPropertySet ps;
	auto device = (KcAudioDevice*)dptr_;

	KvPropertiedObject::KpProperty prop;
	prop.id = kPrivate::k_device_id;
	prop.name = u8"Device";
	prop.desc = u8"device used to capture audio";
	prop.val = QVariant::fromValue<int>(deviceId_); // int类型代表enum类型
	for (unsigned i = 0; i < device->count(); i++) {
		auto info = device->info(i);
		if (info.inputChannels > 0) {
			KvPropertiedObject::KpProperty sub;
			sub.name = QString::number(i);
			sub.disp = QString::fromLocal8Bit(info.name);
			sub.val = static_cast<int>(i);
			prop.children.push_back(sub);
		}
	}
	ps.push_back(prop);

	auto info = device->info(deviceId_);
	assert(channels_ <= info.inputChannels);

	prop.children.clear();
	prop.id = kPrivate::k_channels;
	prop.name = u8"Channles";
	prop.desc = u8"channels of audio input device";
	prop.val = channels_; // int类型代表enum类型
	prop.minVal = 1; 
	prop.maxVal = info.inputChannels; 
	prop.step = 1;
	ps.push_back(prop);

	prop.children.clear();
	prop.id = kPrivate::k_sample_rate;
	prop.name = u8"SampleRate";
	prop.disp = u8"Sampling rate";
	prop.desc = u8"sampling rate of audio input device in Hz";
	prop.val = QVariant::fromValue<int>(sampleRate_);
	for (auto rate : info.sampleRates) {
		KvPropertiedObject::KpProperty sub;
		sub.name = QString::number(rate);
		sub.disp.clear();
		sub.val = static_cast<int>(rate);
		prop.children.push_back(sub);
	}
	ps.push_back(prop);

	prop.children.clear();
	prop.id = kPrivate::k_frame_time;
	prop.name = u8"FrameTime";
	prop.disp = u8"Frame time";
	prop.desc = u8"time in second per frame of audio input";
	prop.val = frameTime_; 
	prop.minVal = 0.005; // 最小5ms
	prop.maxVal = 1.0; // 最大1s
	prop.step = 0.01;
	ps.push_back(prop);

	return ps;
}


void KcAudioInputStream::setPropertyImpl_(int id, const QVariant& newVal)
{
	switch (id) {
	case kPrivate::k_device_id:
		deviceId_ = newVal.toInt();
		break;

	case kPrivate::k_channels:
		channels_ = newVal.toInt();
		break;

	case kPrivate::k_sample_rate:
		sampleRate_ = newVal.toInt();
		break;

	case kPrivate::k_frame_time:
		frameTime_ = newVal.toDouble();
		break;

	default:
		break;
	}
}