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
	assert(prov);

	auto rate = std::round(1.0 / prov->step(port->index(), 0));
	if (rate < 1600)
		return false;

	auto chs = prov->channels(port->index());
	assert(render_->stopped());
	return render_->play(deviceId_, rate, chs, frameTime_);
}


void KcRdAudioPlayer::onStopPipeline()
{
	render_->stop(true);
	render_->reset();
}


void KcRdAudioPlayer::onInput(KcPortNode* outPort, unsigned inPort)
{
	assert(inPort == 0);

	auto pnode = outPort->parent().lock();
	auto prov = std::dynamic_pointer_cast<KvDataProvider>(pnode);
	assert(prov);

	auto data = prov->fetchData(outPort->index());
	if (!data || data->size() == 0)
		return;

	auto samp = std::dynamic_pointer_cast<KvSampled>(data);
	assert(render_ && samp && samp->dim() == 1);

	if (prov->isStream(outPort->index()) ||
		KsImApp::singleton().pipeline().frameIndex() == 0)
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
