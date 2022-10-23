#include "KcPvAudioInput.h"
#include "audio/KcAudio.h"
#include "audio/KcAudioDevice.h"
#include "KtSampling.h"
#include "imgui.h"
#include "KuStrUtil.h"
#include "readerwriterqueue/readerwriterqueue.h"


namespace kPrivate
{
	using data_queue = moodycamel::ReaderWriterQueue<std::shared_ptr<KcSampled1d>>;

    class KcAudioStreamObserver : public KcAudioDevice::observer_type
    {
    public:

        KcAudioStreamObserver(KcPvAudioInput* ais) {
            stream_ = ais;
        }

        bool update(void*/*outputBuffer*/, void* inputBuffer, unsigned frames, double streamTime) override {
            auto data = std::make_shared<KcSampled1d>();
			data->resizeChannel(stream_->channels());
			data->reset(0, streamTime, stream_->step(0));
            data->pushBack(static_cast<kReal*>(inputBuffer), frames);
			stream_->enqueue(data);
            return true;
        }

    private:
        KcPvAudioInput* stream_; 
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
	queue_ = new kPrivate::data_queue;
}


KcPvAudioInput::~KcPvAudioInput()
{
    delete (KcAudioDevice*)dptr_;
	delete (kPrivate::data_queue*)queue_;
}


bool KcPvAudioInput::onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins)
{
	assert(ins.empty());

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
	data_ = std::make_shared<KcSampled1d>(samp.dx(), channels_);
	return device->start();
}


void KcPvAudioInput::onStopPipeline()
{
	((KcAudioDevice*)dptr_)->stop(true);
	while (((kPrivate::data_queue*)queue_)->pop()); // clear the queued audio input data
}


void KcPvAudioInput::output()
{
	if (data_) { // 当pipeline运行时新增该node，data_将为null
		// 组装数据
		auto q = (kPrivate::data_queue*)queue_;
		data_->clear(); // 清空历史数据
		std::shared_ptr<KcSampled1d> d;
		while (q->try_dequeue(d))
			data_->pushBack(*d);
	}
}


std::shared_ptr<KvData> KcPvAudioInput::fetchData(kIndex outPort)
{
	assert(outPort == 0);
	return data_;
}


void KcPvAudioInput::showProperySet()
{
	KvBlockNode::showProperySet();

	auto device = (KcAudioDevice*)dptr_;

	auto info = device->info(deviceId_);
	if (ImGui::BeginCombo("Device", info.name.c_str())) {
		for (unsigned i = 0; i < device->count(); i++) {
			info = device->info(i);
			if (info.inputChannels > 0 && ImGui::Selectable(info.name.c_str(), i == deviceId_))
				deviceId_ = i;
		}

		ImGui::EndCombo();
	}

	info = device->info(deviceId_);
	assert(channels_ <= info.inputChannels);
	ImGui::SliderInt("Channles", &channels_, 1, info.inputChannels);

	auto rateStr = KuStrUtil::toString(sampleRate_);
	if (ImGui::BeginCombo("SampleRate", rateStr.c_str())) {
		for (auto rate : info.sampleRates)
			if (ImGui::Selectable(KuStrUtil::toString(rate).c_str(), sampleRate_ == rate))
				sampleRate_ = rate;

		ImGui::EndCombo();
	}

	ImGui::DragFloat("FrameTime", &frameTime_, 0.01, 0.01, 1.0);

	ImGui::Separator();

	ImGui::LabelText("Dim", "%d", dim());

	ImGui::LabelText("Size", "%d", size(0));

	ImGui::LabelText("Step", "%g", step(0));
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


void KcPvAudioInput::enqueue(const std::shared_ptr<KcSampled1d>& data)
{
	((kPrivate::data_queue*)queue_)->enqueue(data);
}