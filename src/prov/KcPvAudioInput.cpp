#include "KcPvAudioInput.h"
#include "audio/KcAudio.h"
#include "audio/KcAudioDevice.h"
#include "KtSampling.h"


namespace kPrivate
{
    class KcAudioStreamObserver : public KcAudioDevice::observer_type
    {
    public:

        KcAudioStreamObserver(KcPvAudioInput* ais) {
            stream_ = ais;
            dx_ = kReal(1) / ais->sampleRate();
            channles_ = ais->channels();
        }

        bool update(void*/*outputBuffer*/, void* inputBuffer, unsigned frames, double streamTime) override {
            auto data = std::make_shared<KcAudio>();
			data->resizeChannel(channles_);
			data->reset(0, streamTime, dx_);
            data->pushBack(static_cast<kReal*>(inputBuffer), frames);
            emit stream_->pushData(data);
            return true;
        }

    private:
        KcPvAudioInput* stream_; 
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


KcPvAudioInput::KcPvAudioInput() : KvDataProvider("AudioInput")
{
    dptr_ = new KcAudioDevice;
    deviceId_ = ((KcAudioDevice*)dptr_)->defaultInput();
    channels_ = 1;
    sampleRate_ = ((KcAudioDevice*)dptr_)->preferredSampleRate(deviceId_);
    frameTime_ = 0.1;
}


KcPvAudioInput::~KcPvAudioInput()
{
    delete (KcAudioDevice*)dptr_;
}


bool KcPvAudioInput::doStart()
{
    auto device = (KcAudioDevice*)dptr_;
    if (device->opened()) 
        device->close(); // TODO: 若参数一致，则不关闭

    device->remove<kPrivate::KcAudioStreamObserver>(); // 根据当前参数重新添加观察者

    KcAudioDevice::KpStreamParameters iParam;
    iParam.deviceId = deviceId_;
    iParam.channels = channels_;

	KtSampling<kReal> samp(kReal(0), kReal(frameTime_), kReal(1) / sampleRate_, 0);
	unsigned bufferFrames = samp.size(); // unsigned(sampleRate_ * frameTime_ + 0.5);

    if (!device->open(nullptr, &iParam,
        std::is_same<kReal, double>::value ? KcAudioDevice::k_float64 : KcAudioDevice::k_float32,
        sampleRate_, bufferFrames))
        return false;

    device->pushBack(std::make_shared<kPrivate::KcAudioStreamObserver>(this));
    return device->start();
}


void KcPvAudioInput::doStop()
{
    ((KcAudioDevice*)dptr_)->stop(true);
}


bool KcPvAudioInput::isRunning() const
{
	return ((KcAudioDevice*)dptr_)->running();
}


kRange KcPvAudioInput::range(kIndex axis) const
{
	return axis == 0 ? kRange{ 0, frameTime_ } : kRange{ -1, 1 };
}


kReal KcPvAudioInput::step(kIndex axis) const
{
	return axis == 0 ? static_cast<kReal>(1) / sampleRate() : KvDiscreted::k_nonuniform_step;
}


kIndex KcPvAudioInput::size(kIndex) const
{
	KtSampling<kReal> samp(kReal(0), kReal(frameTime_), kReal(1) / sampleRate_, 0);
	return samp.size(); // TODO: 使用open时的bufferFrames参数
}


KcPvAudioInput::kPropertySet KcPvAudioInput::propertySet() const
{
	KvPropertiedObject::kPropertySet ps;
	auto device = (KcAudioDevice*)dptr_;

	KvPropertiedObject::KpProperty prop;
	prop.id = kPrivate::k_device_id;
	prop.name = u8"Device";
	prop.desc = u8"device used to capture audio";
	prop.val = QVariant::fromValue<int>(deviceId_); // int类型代表enum类型
	prop.enumList.clear();
	for (unsigned i = 0; i < device->count(); i++) {
		auto info = device->info(i);
		if (info.inputChannels > 0) 
			prop.enumList.push_back({ QString::fromLocal8Bit(info.name), i });
	}
	ps.push_back(prop);
	prop.enumList.clear();

	auto info = device->info(deviceId_);
	assert(channels_ <= info.inputChannels);

	prop.children.clear();
	prop.id = kPrivate::k_channels;
	prop.name = u8"Channles";
	prop.desc = u8"channels of audio input device";
	prop.val = int(channels_); 
	prop.minVal = 1; 
	prop.maxVal = int(info.inputChannels); 
	prop.step = 1;
	ps.push_back(prop);

	prop.children.clear();
	prop.id = kPrivate::k_sample_rate;
	prop.name = u8"SampleRate";
	prop.disp = u8"Sampling rate";
	prop.desc = u8"sampling rate of audio input device in Hz";
	prop.val = QVariant::fromValue<int>(sampleRate_);
	for (auto rate : info.sampleRates) 
		prop.enumList.push_back({ QString::number(rate), rate });
	ps.push_back(prop);
	prop.enumList.clear();

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


void KcPvAudioInput::setPropertyImpl_(int id, const QVariant& newVal)
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