#pragma once
#include "KvDataRender.h"
#include <memory>

class KcAudioRender;

class KcRdAudioPlayer : public KvDataRender
{
	using super_ = KvDataRender;

public:
	KcRdAudioPlayer();

	bool onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins) override;

	void onStopPipeline() override;

	void onInput(KcPortNode* outPort, unsigned inPort) override;

	bool permitInput(int dataSpec, unsigned inPort) const override;

	void showProperySet() override;

	void onDoubleClicked() override;


private:
	std::unique_ptr<KcAudioRender> render_;
	unsigned deviceId_;
	float frameTime_;
	unsigned dataStamp_; // 当前接收的数据时间戳
};
