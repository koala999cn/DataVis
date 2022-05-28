#include "KcRdAudioPlayer.h"
#include "audio/KcAudioRender.h"
#include "audio/KcAudioDevice.h"
#include "dsp/KcSampled1d.h"


KcRdAudioPlayer::KcRdAudioPlayer(KvDataProvider* is)
    : KvDataRender(tr("AudioPlayer"), is)
{
	render_ = std::make_unique<KcAudioRender>();

	deviceId_ = render_->defaultDevice();
	channels_ = is->channels();
	sampleRate_ = 1 / is->step(0);
	frameTime_ = 0.1;
}


namespace kPrivate
{
	enum KeAudioPlayerPropertyId
	{
		k_device_id,
		k_channels,
		k_sample_rate,
		k_frame_time,
	};
}


KcRdAudioPlayer::kPropertySet KcRdAudioPlayer::propertySet() const
{
	kPropertySet ps;
	KpProperty prop;
	KcAudioDevice dev;

	prop.id = kPrivate::k_device_id;
	prop.name = tr("Device");
	prop.desc = tr("device used to render audio");
	prop.val = QVariant::fromValue<int>(deviceId_); // int类型代表enum类型
	for (unsigned i = 0; i < dev.count(); i++) {
		auto info = dev.info(i);
		if (info.outputChannels > 0) {
			KpProperty sub;
			sub.name = QString::number(i);
			sub.disp = QString::fromLocal8Bit(info.name);
			sub.val = static_cast<int>(i);
			prop.children.push_back(sub);
		}
	}
	ps.push_back(prop);

	auto info = dev.info(deviceId_);
	assert(channels_ <= info.outputChannels);

	prop.children.clear();
	prop.id = kPrivate::k_channels;
	prop.name = tr("Channles");
	prop.desc = tr("channels of audio input device");
	prop.val = channels_; // int类型代表enum类型
	prop.minVal = 1;
	prop.maxVal = info.outputChannels;
	prop.step = 1;
	ps.push_back(prop);

	prop.children.clear();
	prop.id = kPrivate::k_sample_rate;
	prop.name = u8"SampleRate";
	prop.disp = u8"Sampling rate";
	prop.desc = u8"sampling rate of audio output device in Hz";
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
	prop.desc = u8"time in second per frame of audio output";
	prop.val = frameTime_;
	prop.minVal = 0.005; // 最小5ms
	prop.maxVal = 1.0; // 最大1s
	prop.step = 0.01;
	ps.push_back(prop);

	return ps;
}


std::string KcRdAudioPlayer::errorText() const
{
	assert(render_);
	return render_->errorText();
}


bool KcRdAudioPlayer::canShown() const
{
	return false; // TODO: 显示播放进度
}


bool KcRdAudioPlayer::isVisible() const
{
	return false;
}


void KcRdAudioPlayer::reset()
{
    render_->reset();
}


void KcRdAudioPlayer::show(bool bShow)
{
	assert(false);
}


void KcRdAudioPlayer::setPropertyImpl_(int id, const QVariant& newVal)
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


void KcRdAudioPlayer::syncParent()
{
	if (!render_->opened())
		render_->play(deviceId_, sampleRate_, channels_, frameTime_);
}

	
bool KcRdAudioPlayer::renderImpl_(std::shared_ptr<KvData> data)
{
	auto samp1d = std::dynamic_pointer_cast<KcSampled1d>(data);
	assert(render_ && samp1d);
	
	render_->enqueue(samp1d);

	return true;
}
