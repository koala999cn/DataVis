#pragma once
#include "KvDataOperator.h"
#include <memory>
#include "KtFraming.h"


// KcOpFraming不能从KvOpSampled1dHelper继承，因为framing操作符改变了数据维度
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

	bool prepareOutput_() final;

	void outputImpl_() final;

	kIndex frameSize() const;
	void setFrameSize(kIndex s);

	kIndex shiftSize() const;
	void setShiftSize(kIndex s);

private:
	std::unique_ptr<KtFraming<kReal>> framing_;
	float frameTime_; // 单帧时长
	float shiftTime_; // 帧移时长
};

