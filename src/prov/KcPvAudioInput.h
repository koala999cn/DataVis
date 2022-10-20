#pragma once
#include "KvDataProvider.h"

class KcSampled1d;

class KcPvAudioInput : public KvDataProvider
{
public:
	KcPvAudioInput();
	virtual ~KcPvAudioInput();

	bool isStream() const override { return true; }

	kIndex dim() const final { return 1; };

	kIndex channels() const override { return channels_; }

	kRange range(kIndex axis) const final;

	kReal step(kIndex axis) const final;

	kIndex size(kIndex) const final;

	bool onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>&) final;

	void onStopPipeline() final;

	void output() final;

	std::shared_ptr<KvData> fetchData(kIndex outPort) final;

	void showProperySet() override;

	auto sampleRate() const { return sampleRate_; }

	// 将数据插入缓存队列
	void enqueue(const std::shared_ptr<KcSampled1d>& data);

private:
	void* dptr_;
	unsigned deviceId_;
	int channels_;
	unsigned sampleRate_;
	float frameTime_;
	void* queue_; // 缓存队列，暂存捕获的音频数据
	std::shared_ptr<KcSampled1d> data_; // 当前输出数据
};

