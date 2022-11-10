#include "KcRdAudioPlayer.h"
#include "audio/KcAudioRender.h"
#include "prov/KvDataProvider.h" // for KpDataSpec
#include "dsp/KcSampled1d.h"
#include "imgui.h"
#include "imapp/KsImApp.h"
#include "imapp/KgPipeline.h"


KcRdAudioPlayer::KcRdAudioPlayer()
    : KvDataRender("AudioPlayer")
{
	render_ = std::make_unique<KcAudioRender>();
	render_->setAutoStop(false);

	deviceId_ = render_->defaultDevice();
	frameTime_ = 0.1;
}


bool KcRdAudioPlayer::onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins)
{
	if (ins.empty())
		return false;

	assert(ins.front().first == 0);
	auto port = ins.front().second; // TODO: 处理多输入的情况
	auto node = port->parent().lock();
	auto prov = std::dynamic_pointer_cast<KvDataProvider>(node);
	if (!prov)
		return false;

	auto rate = std::round(1.0 / prov->step(port->index(), 0));
	auto chs = prov->channels(port->index());
	assert(render_->stopped());
	return render_->play(deviceId_, rate, chs, frameTime_);
}


void KcRdAudioPlayer::onStopPipeline()
{
	render_->stop(true);
}


void KcRdAudioPlayer::onInput(KcPortNode* outPort, unsigned inPort)
{
	assert(inPort == 0);

	auto pnode = outPort->parent().lock();
	auto prov = std::dynamic_pointer_cast<KvDataProvider>(pnode);
	assert(prov);

	auto data = prov->fetchData(outPort->index());

	auto samp = std::dynamic_pointer_cast<KvSampled>(data);
	assert(render_ && samp && samp->dim() == 1);

	render_->enqueue(samp);
}


bool KcRdAudioPlayer::permitInput(int dataSpec, unsigned inPort) const
{
	assert(inPort == 0); 
	
	KpDataSpec sp(dataSpec);
	return sp.dim == 1 && sp.type == k_sampled;
}


void KcRdAudioPlayer::showProperySet()
{
	super_::showProperySet(); 

	bool disable = KsImApp::singleton().pipeline().running();
	ImGui::BeginDisabled(disable);

	KcAudioDevice dev;
	auto info = dev.info(deviceId_);

	if (ImGui::BeginCombo("Device", info.name.c_str())) {
		for (unsigned i = 0; i < dev.count(); i++) {
			info = dev.info(i);
			if (info.outputChannels > 0 && ImGui::Selectable(info.name.c_str(), i == deviceId_))
				deviceId_ = i;
		}

		ImGui::EndCombo();
	}

	ImGui::DragFloat("FrameTime", &frameTime_, 0.01, 0.01, 1.0);


	ImGui::EndDisabled();
}


void KcRdAudioPlayer::onDoubleClicked()
{

}

/*
KcRdAudioPlayer::kPropertySet KcRdAudioPlayer::propertySet() const
{
	kPropertySet ps;
	KpProperty prop;
	KcAudioDevice dev;

	prop.id = kPrivate::k_device_id;
	prop.name = tr("Device");
	prop.desc = tr("device used to render audio");
	prop.val = int(deviceId_); // int类型代表enum类型
	for (unsigned i = 0; i < dev.count(); i++) {
		auto info = dev.info(i);
		if (info.outputChannels > 0) {
			auto name = QString::fromLocal8Bit(info.name.c_str(), info.name.size());
			prop.enumList.emplace_back(name, i);
		}
	}
	ps.push_back(prop);
	prop.enumList.clear();

	auto pobj = dynamic_cast<KvDataProvider*>(parent());

	prop.id = kPrivate::k_channels;
	prop.name = tr("Channles");
	prop.disp.clear();
	prop.desc = tr("channels of audio output device");
	prop.flag = k_readonly;
	prop.val = int(pobj->channels());
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


*/
