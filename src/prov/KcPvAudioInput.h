#pragma once
#include "KvDataProvider.h"

class KcSampled1d;

class KcPvAudioInput : public KvDataProvider
{
	using super_ = KvDataProvider;

public:
	KcPvAudioInput();
	virtual ~KcPvAudioInput();

	int spec(kIndex outPort) const override;

	kRange range(kIndex outPort, kIndex axis) const final;

	kReal step(kIndex outPort, kIndex axis) const final;

	kIndex size(kIndex outPort, kIndex) const final;

	bool onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>&) final;

	void onStopPipeline() final;

	void output() final;

	std::shared_ptr<KvData> fetchData(kIndex outPort) const final;

	unsigned dataStamp(kIndex outPort) const final;

	void showPropertySet() override;

	auto sampleRate() const { return sampleRate_; }

	// 将数据插入缓存队列
	void enqueue(const std::shared_ptr<KcSampled1d>& data);

private:
	void* dptr_;
	unsigned deviceId_;
	KpDataSpec spec_;
	unsigned sampleRate_;
	float frameTime_;
	void* queue_; // 缓存队列，暂存捕获的音频数据
	std::shared_ptr<KcSampled1d> data_; // 当前输出数据
	unsigned dataStamp_{ 0 };
};

