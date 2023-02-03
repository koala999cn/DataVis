#include "KcRdAudioPlayer.h"
#include "audio/KcAudioRender.h"
#include "prov/KvDataProvider.h" // for KpDataSpec
#include "dsp/KcSampled1d.h"
#include "dsp/KuDataUtil.h"
#include "imapp/KsImApp.h"
#include "imapp/KgPipeline.h"
#include "imgui.h"


KcRdAudioPlayer::KcRdAudioPlayer()
    : KvDataRender("AudioPlayer")
{
	render_ = std::make_unique<KcAudioRender>();
	render_->setAutoStop(false);

	deviceId_ = render_->defaultDevice();
}


bool KcRdAudioPlayer::onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins)
{
	dataStamp_ = 0;

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

	if (prov->dataStamp(outPort->index()) > dataStamp_) { // 只接受更新的数据

		dataStamp_ = prov->dataStamp(outPort->index());

		if (!prov->isStream(outPort->index()) || samp.unique()) {
			render_->enqueue(samp); // 对于独享数据和快照数据，直接压入播放队列
		}
		else { // 对于共享数据，拷贝之后再压入播放队列
			auto samp1d = std::dynamic_pointer_cast<KvSampled>(KuDataUtil::cloneSampled1d(samp));
			assert(samp1d);
			assert(samp1d->size() == samp->size());
			assert(samp1d->step(0) == samp->step(0));
			assert(samp1d->channels() == samp->channels());
			render_->enqueue(samp1d);
		}
	}
}


bool KcRdAudioPlayer::permitInput(int dataSpec, unsigned inPort) const
{
	assert(inPort == 0); 
	
	KpDataSpec sp(dataSpec);
	return sp.dim == 1 && sp.type == k_sampled;
}


namespace kPrivate
{
	std::string localToUtf8(const std::string& str);
}

void KcRdAudioPlayer::showProperySet()
{
	super_::showProperySet(); 

	ImGui::BeginDisabled(working_());

	KcAudioDevice dev;
	auto info = dev.info(deviceId_);

	if (ImGui::BeginCombo("Device", kPrivate::localToUtf8(info.name).c_str())) {
		for (unsigned i = 0; i < dev.count(); i++) {
			info = dev.info(i);
			auto name = kPrivate::localToUtf8(info.name);
			if (info.outputChannels > 0 && ImGui::Selectable(name.c_str(), i == deviceId_))
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
