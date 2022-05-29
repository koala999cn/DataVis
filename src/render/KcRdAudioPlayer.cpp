#include "KcRdAudioPlayer.h"
#include "audio/KcAudioRender.h"
#include "audio/KcAudioDevice.h"
#include "dsp/KcSampled1d.h"


KcRdAudioPlayer::KcRdAudioPlayer(KvDataProvider* is)
    : KvDataRender(tr("AudioPlayer"), is)
{
	render_ = std::make_unique<KcAudioRender>();

	deviceId_ = render_->defaultDevice();
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
	prop.children.clear();

	auto pobj = dynamic_cast<KvDataProvider*>(parent());

	prop.id = kPrivate::k_channels;
	prop.name = tr("Channles");
	prop.disp.clear();
	prop.desc = tr("channels of audio input device");
	prop.flag = k_readonly;
	prop.val = pobj->channels();
	ps.push_back(prop);

	prop.id = kPrivate::k_sample_rate;
	prop.name = tr("SampleRate");
	prop.disp = tr("Sampling rate");
	prop.desc = tr("sampling rate of audio output device in Hz");
	prop.val = 1.0 / pobj->step(0);
	ps.push_back(prop);

	prop.id = kPrivate::k_frame_time;
	prop.name = tr("FrameTime");
	prop.disp = tr("Frame time");
	prop.desc = tr("time in second per frame of audio output");
	prop.flag = 0;
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

	case kPrivate::k_frame_time:
		frameTime_ = newVal.toDouble();
		break;

	default:
		break;
	}
}


void KcRdAudioPlayer::syncParent()
{
	//if (!render_->opened())
	//	render_->play(deviceId_, sampleRate_, channels_, frameTime_);
}

	
bool KcRdAudioPlayer::renderImpl_(std::shared_ptr<KvData> data)
{
	auto samp1d = std::dynamic_pointer_cast<KcSampled1d>(data);
	assert(render_ && samp1d);
	
	render_->enqueue(samp1d);

	if (render_->stopped())
		render_->play(deviceId_, samp1d->sampleRate(), samp1d->channels(), frameTime_);

	return true;
}
