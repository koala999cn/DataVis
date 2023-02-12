#pragma once
#include "KvDataOperator.h"
#include <memory>
#include "KtFraming.h"


class KcOpFraming : public KvDataOperator
{
	using super_ = KvDataOperator;

public:
	KcOpFraming();

	int spec(kIndex outPort) const final;

	kRange range(kIndex outPort, kIndex axis) const final;

	kReal step(kIndex outPort, kIndex axis) const final;

	kIndex size(kIndex outPort, kIndex axis) const final;

	bool onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins) final;

	void onStopPipeline() final;

	void showPropertySet() final;

	bool permitInput(int dataSpec, unsigned inPort) const final;

private:

	bool prepareOutput_() final { return false; }

	void outputImpl_() final;

	kIndex frameSize() const;
	kIndex shiftSize() const;

private:
	std::unique_ptr<KtFraming<kReal>> framing_;
	kReal frameTime_; // 单帧时长
	kReal shiftTime_; // 帧移时长
};

